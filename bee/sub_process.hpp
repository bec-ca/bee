#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "error.hpp"
#include "fd.hpp"
#include "file_path.hpp"
#include "time.hpp"

namespace bee {

struct SubProcess {
 public:
  struct Pipe {
    using ptr = std::shared_ptr<Pipe>;
    Pipe();
    ~Pipe();

    void set_fd(const FD::shared_ptr& fd);

    const FD::shared_ptr& fd() const;

    static ptr create();

   protected:
    FD::shared_ptr _fd;
  };

  struct DefaultIO {};

  struct Pid {
   public:
    static Pid of_int(int pid);

    int to_int() const;

    bool operator<(const Pid& other) const;

    OrError<> kill();

   private:
    explicit Pid(int pid);
    int _pid;
  };

  struct OutputToString {
   public:
    using ptr = std::shared_ptr<OutputToString>;

    static ptr create();

    virtual ~OutputToString();

    virtual OrError<std::string> get_output() = 0;

    virtual void set_fd(const FD::shared_ptr& fd) = 0;
  };

  using output_spec_type =
    std::variant<DefaultIO, Pipe::ptr, FilePath, OutputToString::ptr>;

  using input_spec_type = std::variant<DefaultIO, Pipe::ptr, FilePath>;

  struct CreateProcessArgs {
    const FilePath cmd;
    const std::vector<std::string> args = {};
    input_spec_type stdin_spec = DefaultIO{};
    output_spec_type stdout_spec = DefaultIO{};
    output_spec_type stderr_spec = DefaultIO{};
    const std::optional<FilePath> cwd = std::nullopt;
  };

  using ptr = std::shared_ptr<SubProcess>;

  struct ProcessStatus {
    ptr proc;
    int exit_status;
  };

  SubProcess(const SubProcess& other) = delete;
  SubProcess(SubProcess&& other) = delete;

  ~SubProcess();

  static OrError<ptr> spawn(const CreateProcessArgs& args);

  static OrError<> run(const CreateProcessArgs& args);

  static OrError<std::optional<ProcessStatus>> wait_any(bool block);

  OrError<> wait();

  OrError<> kill();

  explicit SubProcess(Pid pid);

  static int num_running_processes();

  const Pid& pid() const;

 private:
  Pid _pid;
};

} // namespace bee
