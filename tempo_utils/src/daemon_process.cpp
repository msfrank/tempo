
#include <absl/strings/str_cat.h>

#include <tempo_utils/daemon_process.h>
#include <tempo_utils/log_stream.h>
#include <tempo_utils/posix_result.h>

tempo_utils::DaemonProcess::DaemonProcess(
    const ProcessInvoker &invoker,
    const std::filesystem::path &runDirectory,
    std::string_view childOutput,
    int exitStatus)
    : m_invoker(invoker),
      m_runDirectory(runDirectory),
      m_childOutput(childOutput),
      m_exitStatus(exitStatus)
{
    TU_ASSERT (m_invoker.isValid());
}

tempo_utils::ProcessInvoker
tempo_utils::DaemonProcess::getInvoker() const
{
    return m_invoker;
}

std::filesystem::path
tempo_utils::DaemonProcess::getRunDirectory() const
{
    return m_runDirectory;
}

std::string
tempo_utils::DaemonProcess::getChildOutput() const
{
    return m_childOutput;
}

int
tempo_utils::DaemonProcess::getExitStatus() const
{
    return m_exitStatus;
}

#include <sys/wait.h>

static tempo_utils::Status
posix_spawn_daemon(
    const tempo_utils::ProcessInvoker &invoker,
    const std::filesystem::path &runDirectory,
    std::string *childOutput,
    int *exitStatus)
{
    // create the pipe
    int p[2] = {-1,-1};
    auto ret = pipe(p);
    if (ret < 0)
        return tempo_utils::PosixStatus::last("pipe failure");

    // fork child process
    auto pid = fork();
    if (pid < 0)
        return tempo_utils::PosixStatus::last("fork failure");

    /******************************************************************/
    // this is the child
    /******************************************************************/

    if (pid == 0) {

        TU_LOG_INFO << "(child) forked child process";

        if (!runDirectory.empty()) {
            std::error_code ec;
            std::filesystem::current_path(runDirectory, ec);
            TU_LOG_FATAL_IF (ec) << "(child) failed to change directory to " << runDirectory << ": " << ec.message();
            TU_LOG_INFO << "(child) changed current working directory to " << runDirectory;
        }

        auto sid = setsid();
        TU_LOG_FATAL_IF(sid < 0) << "(child) failed to set session: " << strerror(errno);
        TU_LOG_INFO << "(child) set session sid " << sid;

        // redirect pipe write end to stdout
        ret = dup2(p[STDOUT_FILENO], STDOUT_FILENO);
        TU_LOG_FATAL_IF(ret < 0) << "dup2 failed: " << strerror(errno);
        TU_LOG_INFO << "redirected stdout";

        // close the read end of the pipe
        ret = close(p[STDOUT_FILENO]);
        TU_LOG_FATAL_IF(ret < 0) << "(child) failed to close pipe write-end: " << strerror(errno);
        ret = close(p[STDIN_FILENO]);
        TU_LOG_FATAL_IF(ret < 0) << "(child) failed to close pipe read-end: " << strerror(errno);
        TU_LOG_INFO << "(child) closed fds";

        // execute the agent
        TU_LOG_INFO << "(child) executing " << invoker.getExecutable();
        execv(invoker.getExecutable(), invoker.getArgv());

        // if execv fails, then return non-zero
        TU_LOG_FATAL << "(child) failed to exec " << invoker.getExecutable() << ": " << strerror(errno);
        _exit(1);
    }

    /******************************************************************/
    // this is the parent
    /******************************************************************/

    // close the write-end of the pipe
    ret = close(p[STDOUT_FILENO]);
    if (ret < 0)
        return tempo_utils::PosixStatus::last("failed to close pipe write-end");

    // if child output is requested, then read the output from the pipe
    if (childOutput != nullptr) {
        char buf[64];
        for(;;) {
            auto nread = read(p[STDIN_FILENO], buf, 64);
            if (nread < 0) {
                if (errno == EINTR)
                    continue;
                TU_LOG_WARN << "read from pipe failed: " << strerror(errno);
                break;
            }
            if (nread == 0)
                break;
            childOutput->append(buf, nread);
        }
    }

    // close the read end of the pipe
    ret = close(p[STDIN_FILENO]);
    if (ret < 0)
        return tempo_utils::PosixStatus::last("failed to close pipe read-end");

    // wait for the child process to complete
    for (int tries = 3; tries > 0; tries--) {
        int status = -1;
        ret = waitpid(pid, &status, 0);
        if (ret > 0) {
            if (WIFEXITED(status)) {
                *exitStatus = WEXITSTATUS(status);
                break;
            } else if (WIFSIGNALED(status)) {
                return tempo_utils::GenericStatus::forCondition(
                    tempo_utils::GenericCondition::kInternalViolation,
                    "child process {} terminated via signal {}", ret, WTERMSIG(status));
            } else if (WIFSTOPPED(status)) {
                return tempo_utils::GenericStatus::forCondition(
                    tempo_utils::GenericCondition::kInternalViolation,
                    "child process {} stopped via signal {}", ret, WSTOPSIG(status));
            }
            TU_UNREACHABLE();
        }
        if (ret == 0)
            continue;
        if (errno == EINTR)
            continue;
        return tempo_utils::PosixStatus::last("waitpid failed");
    }

    // child process exited, we are done
    return tempo_utils::Status();
}

std::shared_ptr<tempo_utils::DaemonProcess>
tempo_utils::DaemonProcess::spawn(const ProcessInvoker &invoker, const std::filesystem::path &runDirectory)
{
    std::string childOutput;
    int exitStatus;
    auto status = posix_spawn_daemon(invoker, runDirectory, &childOutput, &exitStatus);
    if (status.notOk())
        return {};
    return std::make_shared<DaemonProcess>(invoker, runDirectory, childOutput, exitStatus);
}
