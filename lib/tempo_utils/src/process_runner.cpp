
#include <absl/strings/str_cat.h>
#include <boost/predef.h>

#include <tempo_utils/process_runner.h>
#include <tempo_utils/log_stream.h>
#include <tempo_utils/posix_result.h>

static tempo_utils::Status
run_until_finished(
    const tempo_utils::ProcessInvoker &invoker,
    const std::filesystem::path &runDirectory,
    std::string *childOutput,
    std::string *childError,
    int &exitStatus);

/**
 * Run the process defined by the specified `invoker`. After forking the child process but before
 * running the new executable the current working directory is changed to `runDirectory`.
 *
 * @param invoker Contains the path to the executable and its arguments.
 * @param runDirectory The working directory used when invoking the process.
 */
tempo_utils::ProcessRunner::ProcessRunner(
    const ProcessInvoker &invoker,
    const std::filesystem::path &runDirectory)
    : m_invoker(invoker),
      m_runDirectory(runDirectory),
      m_exitStatus(-1)
{
    TU_ASSERT (m_invoker.isValid());
    m_status = run_until_finished(m_invoker, m_runDirectory, &m_childOutput, &m_childError, m_exitStatus);
}

/**
 *
 * @param invoker
 */
tempo_utils::ProcessRunner::ProcessRunner(const ProcessInvoker &invoker)
    : ProcessRunner(invoker, std::filesystem::current_path())
{
}

bool
tempo_utils::ProcessRunner::isValid() const
{
    return m_status.isOk();
}

tempo_utils::Status
tempo_utils::ProcessRunner::getStatus() const
{
    return m_status;
}

tempo_utils::ProcessInvoker
tempo_utils::ProcessRunner::getInvoker() const
{
    return m_invoker;
}

std::filesystem::path
tempo_utils::ProcessRunner::getRunDirectory() const
{
    return m_runDirectory;
}

std::string
tempo_utils::ProcessRunner::getChildOutput() const
{
    return m_childOutput;
}

std::string
tempo_utils::ProcessRunner::getChildError() const
{
    return m_childError;
}

int
tempo_utils::ProcessRunner::getExitStatus() const
{
    return m_exitStatus;
}

#if BOOST_OS_MACOS || BOOST_OS_LINUX

#include <sys/wait.h>
#include <signal.h>

struct RunnerState {
    int out_p[2] = {-1,-1};
    int err_p[2] = {-1,-1};
    pid_t child_pid = -1;
    pid_t child_sid = -1;
    std::array<char,512> buf;

    ~RunnerState() {
        if (out_p[0] >= 0) { close(out_p[0]); }
        if (out_p[1] >= 0) { close(out_p[1]); }
        if (err_p[0] >= 0) { close(err_p[0]); }
        if (err_p[1] >= 0) { close(err_p[1]); }
        if (child_pid > 0) { kill(child_pid, 9); }
    }
};
static tempo_utils::Status
run_until_finished(
    const tempo_utils::ProcessInvoker &invoker,
    const std::filesystem::path &runDirectory,
    std::string *childOutput,
    std::string *childError,
    int &exitStatus)
{
    RunnerState state;
    int ret;

    // create the pipe for stdout
    ret = pipe(state.out_p);
    if (ret < 0)
        return tempo_utils::PosixStatus::last("failed to allocate pipe for stdout");

    // create the pipe for stderr
    ret = pipe(state.err_p);
    if (ret < 0)
        return tempo_utils::PosixStatus::last("failed to allocate pipe for stderr");

    // fork child process
    state.child_pid = fork();
    if (state.child_pid < 0)
        return tempo_utils::PosixStatus::last("failed to fork child process");

    /******************************************************************/
    // this is the child
    /******************************************************************/

    if (state.child_pid == 0) {
        state.child_pid = getpid();

        if (!runDirectory.empty()) {
            std::error_code ec;
            std::filesystem::current_path(runDirectory, ec);
            TU_LOG_FATAL_IF (ec) << "(pid " << state.child_pid << ") failed to change directory to " << runDirectory << ": " << ec.message();
        }

        state.child_sid = setsid();
        TU_LOG_FATAL_IF(state.child_sid < 0) << "(pid " << state.child_pid << ") failed to set session: " << strerror(errno);

        // redirect pipe write end to stdout
        ret = dup2(state.out_p[STDOUT_FILENO], STDOUT_FILENO);
        TU_LOG_FATAL_IF(ret < 0) << "(pid " << state.child_pid << ") failed to redirect stdout: " << strerror(errno);

        // close the write end of the stdout pipe
        ret = close(state.out_p[STDOUT_FILENO]);
        TU_LOG_FATAL_IF(ret < 0) << "(pid " << state.child_pid << ") failed to close out_p write-end: " << strerror(errno);
        state.out_p[STDOUT_FILENO] = -1;

        // close the read end of the stdout pipe
        ret = close(state.out_p[STDIN_FILENO]);
        TU_LOG_FATAL_IF(ret < 0) << "(pid " << state.child_pid << ") failed to close out_p read-end: " << strerror(errno);
        state.out_p[STDIN_FILENO] = -1;

        // redirect pipe write end to stderr
        ret = dup2(state.err_p[STDOUT_FILENO], STDERR_FILENO);
        TU_LOG_FATAL_IF(ret < 0) << "(pid " << state.child_pid << ") failed to redirect stderr: " << strerror(errno);

        // close the write end of the stderr pipe
        ret = close(state.err_p[STDOUT_FILENO]);
        TU_LOG_FATAL_IF(ret < 0) << "(pid " << state.child_pid << ") failed to close err_p write-end: " << strerror(errno);
        state.err_p[STDOUT_FILENO] = -1;

        // close the read end of the stderr pipe
        ret = close(state.err_p[STDIN_FILENO]);
        TU_LOG_FATAL_IF(ret < 0) << "(pid " << state.child_pid << ") failed to close err_p read-end: " << strerror(errno);
        state.err_p[STDIN_FILENO] = -1;

        // execute the process image
        execv(invoker.getExecutable(), invoker.getArgv());

        // if execv fails, then return non-zero
        TU_LOG_FATAL << "(pid " << state.child_pid << ") failed to exec " << invoker.getExecutable() << ": " << strerror(errno);

        // shouldn't ever get here
        _exit(1);
    }

    /******************************************************************/
    // this is the parent
    /******************************************************************/

    // close the write-end of the stdout pipe
    ret = close(state.out_p[STDOUT_FILENO]);
    if (ret < 0)
        return tempo_utils::PosixStatus::last("failed to close stdout pipe write-end");
    state.out_p[STDOUT_FILENO] = -1;

    // close the write-end of the stderr pipe
    ret = close(state.err_p[STDOUT_FILENO]);
    if (ret < 0)
        return tempo_utils::PosixStatus::last("failed to close stderr pipe write-end");
    state.err_p[STDOUT_FILENO] = -1;

    // read completely from output and error pipes
    bool outputRemaining = true, errorRemaining = true;
    while (outputRemaining || errorRemaining) {
        ssize_t nread;

        if (outputRemaining) {
            nread = read(state.out_p[STDIN_FILENO], state.buf.data(), state.buf.size());
            if (nread < 0) {
                if (errno != EINTR)
                    return tempo_utils::PosixStatus::last("failed to read from stdout pipe");
                // otherwise fall through and retry read
            } else if (nread > 0) {
                childOutput->append(state.buf.data(), nread);
            } else {
                outputRemaining = false;
            }
        }

        if (errorRemaining) {
            nread = read(state.err_p[STDIN_FILENO], state.buf.data(), state.buf.size());
            if (nread < 0) {
                if (errno != EINTR)
                    return tempo_utils::PosixStatus::last("failed to read from stderr pipe");
                // otherwise fall through and retry read
            } else if (nread > 0) {
                childError->append(state.buf.data(), nread);
            } else {
                errorRemaining = false;
            }
        }
    }

    // close the read end of the stdout pipe
    ret = close(state.out_p[STDIN_FILENO]);
    if (ret < 0)
        return tempo_utils::PosixStatus::last("failed to close stdout pipe read-end");
    state.out_p[STDIN_FILENO] = -1;

    // close the read end of the stderr pipe
    ret = close(state.err_p[STDIN_FILENO]);
    if (ret < 0)
        return tempo_utils::PosixStatus::last("failed to close stderr pipe read-end");
    state.err_p[STDIN_FILENO] = -1;

    // wait for the child process to complete
    int status = -1;
    for (int tries = 3; tries > 0; tries--) {
        ret = waitpid(state.child_pid, &status, 0);
        if (ret > 0)
            break;
        if (ret == 0)
            continue;
        if (errno == EINTR)
            continue;
        return tempo_utils::PosixStatus::last("failure waiting for process termination");
    }
    state.child_pid = -1;

    // child process exited, we are done
    if (WIFEXITED(status)) {
        exitStatus = WEXITSTATUS(status);
        return {};
    }

    if (WIFSIGNALED(status))
        return tempo_utils::GenericStatus::forCondition(
            tempo_utils::GenericCondition::kInternalViolation,
            "child process {} terminated via signal {}", ret, WTERMSIG(status));

    if (WIFSTOPPED(status))
        return tempo_utils::GenericStatus::forCondition(
            tempo_utils::GenericCondition::kInternalViolation,
            "child process {} stopped via signal {}", ret, WSTOPSIG(status));

    // shouldn't get here
    return tempo_utils::GenericStatus::forCondition(
        tempo_utils::GenericCondition::kInternalViolation,
        "child process {} completed unexpectedly", ret);
}

#endif