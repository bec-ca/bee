#include "sub_process.hpp"

#include "file_descriptor.hpp"
#include "format.hpp"
#include "string_util.hpp"
#include "util.hpp"

#include <cerrno>
#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <future>
#include <map>
#include <mutex>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

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

  virtual void set_fd(const FileDescriptor::shared_ptr& fd) override
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

  static void _reader_thread(
    promise<OrError<string>> prom, FileDescriptor::shared_ptr fd)
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

  FileDescriptor::shared_ptr _fd;
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
        bail(file, FileDescriptor::create_file(filename));
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
        static_assert(always_false_v<T> && "non exaustive visit");
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
        bail(file, FileDescriptor::open_file(filename));
        return Pipe{
          .read_fd = std::move(file).to_shared(),
          .write_fd = nullptr,
        };
      } else if constexpr (std::is_same_v<T, SubProcess::Pipe::ptr>) {
        bail(pipe, sys_pipe());
        input_spec->set_fd(pipe.write_fd);
        return pipe;
      } else {
        static_assert(always_false_v<T> && "non exaustive visit");
      }
    },
    spec);
}

OrError<Unit> prep_output_on_child(
  Pipe& output, const FileDescriptor::shared_ptr& child_fd)
{
  if (output.write_fd != nullptr) {
    bail_unit(output.write_fd->dup_onto(*child_fd));
  }
  return unit;
}

OrError<Unit> prep_input_on_child(
  Pipe& input, const FileDescriptor::shared_ptr& child_fd)
{
  if (input.read_fd != nullptr) {
    bail_unit(input.read_fd->dup_onto(*child_fd));
  }
  return unit;
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
    auto it = m.find(pid);
    if (it == m.end()) { return Error::format("No such pid: $", pid.to_int()); }
    return it->second;
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

} // namespace

////////////////////////////////////////////////////////////////////////////////
// Pipe
//

SubProcess::Pipe::Pipe() : _fd(nullptr) {}

SubProcess::Pipe::~Pipe() {}

void SubProcess::Pipe::set_fd(const FileDescriptor::shared_ptr& fd)
{
  _fd = fd;
}

const FileDescriptor::shared_ptr& SubProcess::Pipe::fd() const { return _fd; }

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
// Pid
//

SubProcess::Pid::Pid(int pid) : _pid(pid) {}

SubProcess::Pid SubProcess::Pid::of_int(int pid) { return Pid(pid); }

int SubProcess::Pid::to_int() const { return _pid; }

bool SubProcess::Pid::operator<(const Pid& other) const
{
  return _pid < other._pid;
}

OrError<Unit> SubProcess::Pid::kill()
{
  if (::kill(_pid, SIGKILL) == 0) { return ok(); }

  return Error::format("Failed to send sigkill to process: $", strerror(errno));
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
  cargs.push_back(args.cmd.c_str());
  for (const auto& arg : args.args) { cargs.push_back(arg.c_str()); }
  cargs.push_back(nullptr);

  bail(stdout_fd_pair, prep_output_spec(args.stdout_spec));
  bail(stderr_fd_pair, prep_output_spec(args.stderr_spec));
  bail(stdin_fd_pair, prep_input_spec(args.stdin_spec));

  int pid = fork();
  if (pid == 0) {
#ifdef __APPLE__
    // TODO: alternative required for aaple
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

    must_unit(
      prep_output_on_child(stdout_fd_pair, FileDescriptor::stdout_filedesc()));
    must_unit(
      prep_output_on_child(stderr_fd_pair, FileDescriptor::stderr_filedesc()));
    must_unit(
      prep_input_on_child(stdin_fd_pair, FileDescriptor::stdin_filedesc()));

    char* const* ccargs = const_cast<char* const*>(cargs.data());
    int ret = execvp(args.cmd.c_str(), ccargs);
    print_err_line(
      "Command failed with code ret:$ errno:$ message:'$' cmd:'$' '$'",
      ret,
      errno,
      strerror(errno),
      args.cmd,
      join(args.args, " "));
    exit(1);
  } else if (pid == -1) {
    return Error::format("Failed to fork process: $", strerror(errno));
  }

  return RunningProcesses::singleton().create_process(Pid::of_int(pid));
}

OrError<Unit> SubProcess::run(const SubProcess::CreateProcessArgs& args)
{
  bail(proc, spawn(args));
  return proc->wait();
}

OrError<std::optional<SubProcess::ProcessStatus>> SubProcess::wait_any(
  bool block)
{
  int flags = 0;
  if (!block) { flags = WNOHANG; }
  int wstatus;
  pid_t int_pid = waitpid(-1, &wstatus, flags);
  if (int_pid == 0) {
    return std::nullopt;
  } else if (int_pid == -1) {
    if (errno == ECHILD) { return std::nullopt; }
    return Error::format("Failed to wait for process: $", strerror(errno));
  }
  int exit_status = WEXITSTATUS(wstatus);
  auto pid = Pid::of_int(int_pid);

  bail(proc, RunningProcesses::singleton().find_by_pid(pid));
  RunningProcesses::singleton().process_ended(pid);

  return SubProcess::ProcessStatus{.proc = proc, .exit_status = exit_status};
}

OrError<Unit> SubProcess::wait()
{
  int wstatus = 0;
  auto ret = waitpid(_pid.to_int(), &wstatus, 0);
  if (ret == 0) {
    return Error::format("waipid unexpectedly returned 0");
  } else if (ret < 0) {
    return Error::format("waipid returned error: $", strerror(errno));
  } else if (ret != _pid.to_int()) {
    return Error::format(
      "waipid returned a value different than the expected pid");
  }
  RunningProcesses::singleton().process_ended(_pid);
  if (WIFEXITED(wstatus)) {
    int exit_status = WEXITSTATUS(wstatus);
    if (exit_status != 0) {
      return Error::format("Process exited with status $", exit_status);
    } else {
      return unit;
    }
  } else if (WIFSIGNALED(wstatus)) {
    return Error::format(
      "Process killed by signal $", strsignal(WTERMSIG(wstatus)));
  } else if (WIFSTOPPED(wstatus)) {
    return Error::format(
      "Process stopped by signal $", strsignal(WSTOPSIG(wstatus)));
  } else if (WIFCONTINUED(wstatus)) {
    return Error::format("Process continued");
  } else {
    return Error::format("Process ended abnormally with wstatus $", wstatus);
  }
}

OrError<Unit> SubProcess::kill() { return _pid.kill(); }

const SubProcess::Pid& SubProcess::pid() const { return _pid; }

int SubProcess::num_running_processes()
{
  return RunningProcesses::singleton().num_running_processes();
}

} // namespace bee
