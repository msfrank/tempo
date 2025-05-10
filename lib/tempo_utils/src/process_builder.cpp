
#include <tempo_utils/process_builder.h>

/**
 * Construct a ProcessBuilder for the given executable.
 *
 * @param executable The path to the executable.
 */
tempo_utils::ProcessBuilder::ProcessBuilder(const std::filesystem::path &executable)
{
    m_exe = std::filesystem::absolute(executable);
    appendArg(m_exe.c_str());
}

/**
 * Construct a ProcessBuilder for the given executable.
 *
 * @param executable The path to the executable.
 * @param arg0 The first argument given to the executable.
 */
tempo_utils::ProcessBuilder::ProcessBuilder(const std::filesystem::path &executable, std::string_view arg0)
{
    m_exe = std::filesystem::absolute(executable);
    appendArg(arg0);
}

/**
 * Append a single argument. This form is typically used for flag arguments, but can also be used when
 * specifying an option argument when there is no space between the option and its value.
 *
 * @param arg The argument.
 */
void
tempo_utils::ProcessBuilder::appendArg(std::string_view arg)
{
    m_args.push_back(std::string(arg));
}

/**
 * Append an argument composed of an option and a value, which are represented as two separate argument strings.
 *
 * @param option The argument option name.
 * @param value  The argument option value.
 */
void
tempo_utils::ProcessBuilder::appendArg(std::string_view option, std::string_view value)
{
    appendArg(option);
    appendArg(value);
}

/**
 * Construct a `ProcessInvoker` from the executable path and arguments.
 *
 * @return The ProcessInvoker.
 */
tempo_utils::ProcessInvoker
tempo_utils::ProcessBuilder::toInvoker() const
{
    return ProcessInvoker(m_exe, m_args);
}

tempo_utils::ProcessInvoker::ProcessInvoker()
{
    m_argv = (char **) malloc(sizeof(char *));
    m_argv[0] = nullptr;
    m_argc = 0;
}

tempo_utils::ProcessInvoker::ProcessInvoker(
    const std::filesystem::path &exe,
    const std::vector<std::string> &args)
{
    m_exe = exe;
    m_argc = args.size();
    m_argv = (char **) calloc(m_argc + 1, sizeof(char *));
    for (int i = 0; i < m_argc; i++) {
        auto &s = args.at(i);
        auto *arg = (char *) malloc(s.size() + 1);
        strncpy(arg, s.c_str(), s.size());
        arg[s.size()] = '\0';
        m_argv[i] = arg;
    }
    m_argv[m_argc] = nullptr;
}

tempo_utils::ProcessInvoker::ProcessInvoker(const ProcessInvoker &other)
{
    m_exe = other.m_exe;
    m_argc = other.m_argc;
    m_argv = (char **) calloc(m_argc + 1, sizeof(char *));
    for (int i = 0; i < m_argc; i++) {
        auto *s = other.m_argv[i];
        auto len = strlen(s);
        auto *arg = (char *) malloc(len + 1);
        strncpy(arg, s, len);
        arg[len] = '\0';
        m_argv[i] = arg;
    }
    m_argv[m_argc] = nullptr;
}

tempo_utils::ProcessInvoker::~ProcessInvoker()
{
    for (int i = 0; i < m_argc; i++) {
        TU_ASSERT (m_argv[i] != nullptr);
        free(m_argv[i]);
    }
    free(m_argv);
}

bool
tempo_utils::ProcessInvoker::isValid() const
{
    return !m_exe.empty();
}

const char *
tempo_utils::ProcessInvoker::getExecutable() const
{
    if (!m_exe.empty())
        return m_exe.c_str();
    return nullptr;
}

std::filesystem::path
tempo_utils::ProcessInvoker::getExecutablePath() const
{
    return m_exe;
}

const char *
tempo_utils::ProcessInvoker::getArg(int index) const
{
    if (index < m_argc)
        return m_argv[index];
    return nullptr;
}

int
tempo_utils::ProcessInvoker::getArgc() const
{
    return m_argc;
}

char **
tempo_utils::ProcessInvoker::getArgv() const
{
    return m_argv;
}

tempo_utils::LogMessage&& tempo_utils::operator<<(
    tempo_utils::LogMessage &&message,
    const tempo_utils::ProcessInvoker &invoker)
{
    if (invoker.isValid()) {
        message.m_buffer << "ProcessInvoker(" << invoker.getArg(0);
        for (int i = 1; i < invoker.getArgc(); i++) {
            message.m_buffer << " " << invoker.getArg(i);
        }
        message.m_buffer << ")";
    } else {
        message.m_buffer << "ProcessInvoker()";
    }
    return std::move(message);
}
