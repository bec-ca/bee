#include "sub_process.hpp"
#include "testing.hpp"

namespace bee {
namespace {

void log()
{
  P("num_running_processes: $", SubProcess::num_running_processes());
}

TEST(run_proc)
{
  log();
  must_unit(SubProcess::run({.cmd = FilePath("/bin/echo"), .args = {"hello"}}));
  log();
  must_unit(
    SubProcess::run({.cmd = FilePath("pwd"), .cwd = FilePath("/usr/bin")}));
  log();
}

TEST(spawn_proc_stdout_spec)
{
  auto output = SubProcess::Pipe::create();
  log();
  must(
    proc,
    SubProcess::spawn(
      {.cmd = FilePath("echo"), .args = {"yo"}, .stdout_spec = output}));
  log();
  char data[1024];
  must(ret, output->fd()->read(reinterpret_cast<std::byte*>(data), 100));
  data[ret.bytes_read()] = 0;
  must_unit(proc->wait());
  P(data);
  log();
}

TEST(spawn_proc_stdin_spec)
{
  log();
  auto input = SubProcess::Pipe::create();
  must(proc, SubProcess::spawn({.cmd = FilePath("cat"), .stdin_spec = input}));
  log();
  std::string msg = "secret code 123\n";
  must(bytes, input->fd()->write(msg));
  assert(bytes == msg.size());
  input->fd()->close();
  must_unit(proc->wait());
  log();
}

TEST(spawn_wait_any)
{
  log();

  must(proc1, SubProcess::spawn({.cmd = FilePath("sleep"), .args = {"0.2"}}));

  log();

  must(proc2, SubProcess::spawn({.cmd = FilePath("sleep"), .args = {"0.4"}}));

  log();

  must(w1, SubProcess::wait_any(true));

  log();

  must(w2, SubProcess::wait_any(true));

  log();

  P(proc1 == w1->proc);
  P(proc2 == w2->proc);
}

TEST(output_to_string)
{
  auto output = SubProcess::OutputToString::create();
  must_unit(SubProcess::run(
    {.cmd = FilePath("echo"), .args = {"yo"}, .stdout_spec = output}));
  P("output: '$'", output->get_output());
}

} // namespace
} // namespace bee
