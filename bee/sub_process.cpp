#include "sub_process.hpp"

#include <cerrno>
#include <csignal>
#include <cstring>
#include <future>
#include <map>
#include <mutex>
#include <thread>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "errno_msg.hpp"
#include "fd.hpp"
#include "format.hpp"
#include "print.hpp"
#include "string_util.hpp"
#include "util.hpp"

#ifndef __APPLE__
#include <sys/prctl.h>
#endif

using std::promise;
using std::string;
using std::thread;

namespace bee {
namespace {

struct OutputToStringImpl : public SubProcess::OutputToString {
 public:
  OutputToStringImpl() {}

  virtual ~OutputToStringImpl()
  {
    _fd->close();
    _maybe_join();
  }

  virtual void set_fd(const FD::shared_ptr& fd) override
  {
    _fd = fd;
    promise<OrError<string>> prom;
    _output = prom.get_future();
    _thread = thread(_reader_thread, std::move(prom), _fd);
  }

  virtual OrError<string> get_output() override
  {
    _maybe_join();
    return _output.get();
  }

 private:
  void _maybe_join()
  {
    if (_thread.has_value()) {
      _thread->join();
      _thread = std::nullopt;
    }
  }

  static void _reader_thread(promise<OrError<string>> prom, FD::shared_ptr fd)
  {
    auto fn = [&fd]() -> OrError<string> {
      DataBuffer buffer;
      while (true) {
        bail(r, fd->read_all_available(buffer));
        if (r.is_eof()) { break; }
      }
      return buffer.to_string();
    };
    prom.set_value(fn());
  }

  std::future<OrError<string>> _output;
  std::optional<thread> _thread;
  FD::shared_ptr _fd;
};

struct InputFromStringImpl : public SubProcess::InputFromString {
 public:
  InputFromStringImpl(const std::string_view& input) : _input(input) {}

  virtual ~InputFromStringImpl()
  {
    _fd->close();
    _maybe_join();
  }

  virtual void set_fd(const FD::shared_ptr& fd) override
  {
    _fd = fd;
    promise<OrError<>> prom;
    _result = prom.get_future();
    _thread = thread(_writer_thread, std::move(prom), _fd, std::move(_input));
  }

  virtual bee::OrError<> result() override { return _result.get(); }

 private:
  void _maybe_join()
  {
    if (_thread.has_value()) {
      _thread->join();
      _thread = std::nullopt;
    }
  }

  static void _writer_thread(
    promise<OrError<>> prom, FD::shared_ptr fd, std::string input)
  {
    prom.set_value(fd->write(input).ignore_value());
    fd->close();
  }

  string _input;
  std::future<OrError<>> _result;
  std::optional<thread> _thread;
  FD::shared_ptr _fd;
};

OrError<Pipe> sys_pipe() { return Pipe::create(); }

OrError<Pipe> prep_output_spec(const SubProcess::output_spec_type& spec)
{
  return visit(
    [&](auto& output_spec) -> OrError<Pipe> {
      using T = std::decay_t<decltype(output_spec)>;
      if constexpr (std::is_same_v<T, SubProcess::DefaultIO>) {
        // Nothing to do here
        return Pipe{
          .read_fd = nullptr,
          .write_fd = nullptr,
        };
      } else if constexpr (std::is_same_v<T, FilePath>) {
        auto& filename = output_spec;
        bail(file, FD::create_file(filename));
        return Pipe{
          .read_fd = nullptr,
          .write_fd = std::move(file).to_shared(),
        };
      } else if constexpr (
        std::is_same_v<T, SubProcess::Pipe::ptr> ||
        std::is_same_v<T, SubProcess::OutputToString::ptr>) {
        bail(pipe, sys_pipe());
        output_spec->set_fd(pipe.read_fd);
        return pipe;
      } else {
        static_assert(always_false<T> && "non exhaustive visit");
      }
    },
    spec);
}

OrError<Pipe> prep_input_spec(const SubProcess::input_spec_type& spec)
{
  return visit(
    [&](auto& input_spec) -> OrError<Pipe> {
      using T = std::decay_t<decltype(input_spec)>;
      if constexpr (std::is_same_v<T, SubProcess::DefaultIO>) {
        // Nothing to do here
        return Pipe{
          .read_fd = nullptr,
          .write_fd = nullptr,
        };
      } else if constexpr (std::is_same_v<T, FilePath>) {
        auto& filename = input_spec;
        bail(file, FD::open_file(filename));
        return Pipe{
          .read_fd = std::move(file).to_shared(),
          .write_fd = nullptr,
        };
      } else if constexpr (
        std::is_same_v<T, SubProcess::Pipe::ptr> ||
        std::is_same_v<T, SubProcess::InputFromString::ptr>) {
        bail(pipe, sys_pipe());
        input_spec->set_fd(pipe.write_fd);
        return pipe;
      } else {
        static_assert(always_false<T> && "non exhaustive visit");
      }
    },
    spec);
}

OrError<> prep_output_on_child(Pipe& output, const FD::shared_ptr& child_fd)
{
  if (output.write_fd != nullptr) {
    bail_unit(output.write_fd->dup_onto(*child_fd));
  }
  return ok();
}

OrError<> prep_input_on_child(Pipe& input, const FD::shared_ptr& child_fd)
{
  if (input.read_fd != nullptr) {
    bail_unit(input.read_fd->dup_onto(*child_fd));
  }
  return ok();
}

struct RunningProcesses {
 public:
  SubProcess::ptr create_process(SubProcess::Pid pid)
  {
    const std::lock_guard lock(_mutex);
    auto ptr = std::make_shared<SubProcess>(pid);
    _running_processes.emplace(pid, ptr);
    return ptr;
  }

  OrError<SubProcess::ptr> find_by_pid(SubProcess::Pid pid)
  {
    const std::lock_guard<std::mutex> lock(_mutex);
    auto& m = _running_processes;
    if (auto it = m.find(pid); it == m.end()) {
      return EF("No such pid: $", pid.to_int());
    } else {
      return it->second;
    }
  }

  void process_ended(SubProcess::Pid pid)
  {
    const std::lock_guard lock(_mutex);
    _running_processes.erase(pid);
  }

  int num_running_processes()
  {
    const std::lock_guard lock(_mutex);
    return _running_processes.size();
  }

  static RunningProcesses& singleton()
  {
    call_once(_singleton_flag, []() {
      _singleton = std::make_unique<RunningProcesses>();
    });
    return *_singleton;
  }

 private:
  static std::unique_ptr<RunningProcesses> _singleton;
  static std::once_flag _singleton_flag;
  std::map<SubProcess::Pid, SubProcess::ptr> _running_processes;
  std::mutex _mutex;
};

std::unique_ptr<RunningProcesses> RunningProcesses::_singleton;
std::once_flag RunningProcesses::_singleton_flag;

OrError<> exit_status_to_or_error(int status)
{
  if (WIFEXITED(status)) {
    int exit_status = WEXITSTATUS(status);
    if (exit_status != 0) {
      return Error::fmt("Process exited with exit status $", exit_status);
    } else {
      return ok();
    }
  } else if (WIFSIGNALED(status)) {
    return Error::fmt(
      "Process killed by signal $", strsignal(WTERMSIG(status)));
  } else if (WIFSTOPPED(status)) {
    return Error::fmt(
      "Process stopped by signal $", strsignal(WSTOPSIG(status)));
  } else if (WIFCONTINUED(status)) {
    return Error::fmt("Process continued");
  } else {
    return Error::fmt("Process ended abnormally with exit status $", status);
  }
}

} // namespace

////////////////////////////////////////////////////////////////////////////////
// Pipe
//

SubProcess::Pipe::Pipe() : _fd(nullptr) {}

SubProcess::Pipe::~Pipe() {}

void SubProcess::Pipe::set_fd(const FD::shared_ptr& fd) { _fd = fd; }

const FD::shared_ptr& SubProcess::Pipe::fd() const { return _fd; }

SubProcess::Pipe::ptr SubProcess::Pipe::create()
{
  return std::make_shared<Pipe>();
}

////////////////////////////////////////////////////////////////////////////////
// OutputToString
//

SubProcess::OutputToString::~OutputToString() {}

SubProcess::OutputToString::ptr SubProcess::OutputToString::create()
{
  return std::make_shared<OutputToStringImpl>();
}

////////////////////////////////////////////////////////////////////////////////
// OutputToString
//

SubProcess::InputFromString::~InputFromString() {}

SubProcess::InputFromString::ptr SubProcess::InputFromString::create(
  const std::string_view& input)
{
  return std::make_shared<InputFromStringImpl>(input);
}

////////////////////////////////////////////////////////////////////////////////
// Pid
//

SubProcess::Pid::Pid(int pid) : _pid(pid) {}

SubProcess::Pid SubProcess::Pid::of_int(int pid) { return Pid(pid); }

int SubProcess::Pid::to_int() const { return _pid; }

bool SubProcess::Pid::operator<(const Pid& other) const
{
  return _pid < other._pid;
}

OrError<> SubProcess::Pid::kill()
{
  if (::kill(_pid, SIGKILL) == 0) { return ok(); }

  return Error::fmt("Failed to send sigkill to process: $", errno_msg());
}

////////////////////////////////////////////////////////////////////////////////
// ProcessStatus
//

OrError<> SubProcess::ProcessStatus::to_or_error() const
{
  return exit_status_to_or_error(exit_status);
}

////////////////////////////////////////////////////////////////////////////////
// SubProcess
//

SubProcess::SubProcess(Pid pid) : _pid(pid) {}

SubProcess::~SubProcess() {}

OrError<SubProcess::ptr> SubProcess::spawn(
  const SubProcess::CreateProcessArgs& args)
{
  std::vector<const char*> cargs;
  cargs.push_back(args.cmd.data());
  for (const auto& arg : args.args) { cargs.push_back(arg.c_str()); }
  cargs.push_back(nullptr);

  bail(stdout_fd_pair, prep_output_spec(args.stdout_spec));
  bail(stderr_fd_pair, prep_output_spec(args.stderr_spec));
  bail(stdin_fd_pair, prep_input_spec(args.stdin_spec));

  int pid = fork();
  if (pid == 0) {
#ifdef __APPLE__
    // TODO: alternative required for apple
#else
    int r = prctl(PR_SET_PDEATHSIG, SIGTERM);
    if (r == -1) {
      perror(0);
      exit(1);
    }
#endif

    if (getppid() == 1) { exit(1); }

    if (args.cwd.has_value()) {
      std::filesystem::current_path(args.cwd->to_std_path());
    }

    must_unit(prep_output_on_child(stdout_fd_pair, FD::stdout_filedesc()));
    must_unit(prep_output_on_child(stderr_fd_pair, FD::stderr_filedesc()));
    must_unit(prep_input_on_child(stdin_fd_pair, FD::stdin_filedesc()));

    char* const* ccargs = const_cast<char* const*>(cargs.data());
    int ret = execvp(args.cmd.data(), ccargs);
    PE(
      "Command failed with code ret:$ errno:$ message:'$' cmd:'$' '$'",
      ret,
      errno,
      errno_msg(),
      args.cmd,
      join(args.args, " "));
    exit(1);
  } else if (pid == -1) {
    return Error::fmt("Failed to fork process: $", errno_msg());
  }

  return RunningProcesses::singleton().create_process(Pid::of_int(pid));
}

OrError<> SubProcess::run(const SubProcess::CreateProcessArgs& args)
{
  bail(proc, spawn(args));
  return proc->wait();
}

OrError<std::optional<SubProcess::ProcessStatus>> SubProcess::wait_any(
  bool block)
{
  int flags = 0;
  if (!block) { flags = WNOHANG; }
  int status;
  pid_t int_pid = waitpid(-1, &status, flags);
  if (int_pid == 0) {
    return std::nullopt;
  } else if (int_pid == -1) {
    if (errno == ECHILD) { return std::nullopt; }
    return Error::fmt("Failed to wait for process: $", errno_msg());
  }
  int exit_status = WEXITSTATUS(status);
  auto pid = Pid::of_int(int_pid);

  bail(proc, RunningProcesses::singleton().find_by_pid(pid));
  RunningProcesses::singleton().process_ended(pid);

  return SubProcess::ProcessStatus{.proc = proc, .exit_status = exit_status};
}

OrError<> SubProcess::wait()
{
  int status = 0;
  auto ret = waitpid(_pid.to_int(), &status, 0);
  if (ret == 0) {
    return Error::fmt("waitpid unexpectedly returned 0");
  } else if (ret < 0) {
    return Error::fmt("waitpid returned error: $", errno_msg());
  } else if (ret != _pid.to_int()) {
    return Error::fmt(
      "waitpid returned a value different than the expected pid");
  }
  RunningProcesses::singleton().process_ended(_pid);
  return exit_status_to_or_error(status);
}

OrError<> SubProcess::kill() { return _pid.kill(); }

const SubProcess::Pid& SubProcess::pid() const { return _pid; }

int SubProcess::num_running_processes()
{
  return RunningProcesses::singleton().num_running_processes();
}

} // namespace bee
