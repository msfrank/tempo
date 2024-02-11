
#include <tempo_utils/log_console.h>

tempo_utils::LogStdout::~LogStdout()
{
    if (m_severity == LogSeverity::kConsoleStdout) {
        auto log = std::move(*m_buffer.rdbuf());
        if (!write_console(m_severity, log.str()))
            std::abort();
    }
}

tempo_utils::LogStderr::~LogStderr()
{
    if (m_severity == LogSeverity::kConsoleStderr) {
        auto log = std::move(*m_buffer.rdbuf());
        if (!write_console(m_severity, log.str()))
            std::abort();
    }
}