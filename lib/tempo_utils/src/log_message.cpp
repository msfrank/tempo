
#include <absl/time/clock.h>

#include <tempo_utils/log_message.h>

tempo_utils::LogMessage::LogMessage(LogMessage &&other) noexcept
{
    buffer = std::move(other.buffer);
}

tempo_utils::LogMessage&
tempo_utils::LogMessage::operator=(LogMessage &&other) noexcept
{
    if (this != &other) {
        buffer = std::move(other.buffer);
    }
    return *this;
}

tempo_utils::LogToString::LogToString(std::string *output)
    : LogMessage(),
      m_output(output)
{
    TU_NOTNULL (output);
}

tempo_utils::LogToString::LogToString(LogToString &&other) noexcept
{
    m_output = other.m_output;
    other.m_output = nullptr;
}

tempo_utils::LogToString::~LogToString()
{
    m_output->append(buffer.str());
}

tempo_utils::LogToString&
tempo_utils::LogToString::operator=(LogToString &&other) noexcept
{
    if (this != &other) {
        m_output = other.m_output;
        other.m_output = nullptr;
    }
    return *this;
}

tempo_utils::ConditionalLogMessage::ConditionalLogMessage(char const *filePath, int lineNr, LogSeverity severity, bool enabled)
    : LogMessage(),
      m_filePath(filePath),
      m_lineNr(lineNr),
      m_severity(severity),
      m_category(nullptr),
      m_enabled(enabled),
      m_ts(absl::Now())
{
}

tempo_utils::ConditionalLogMessage::ConditionalLogMessage(
    char const *filePath,
    int lineNr,
    LogSeverity severity,
    LogCategory const *category,
    bool enabled)
    : LogMessage(),
      m_filePath(filePath),
      m_lineNr(lineNr),
      m_severity(severity),
      m_category(category),
      m_enabled(enabled),
      m_ts(absl::Now())
{
}

tempo_utils::ConditionalLogMessage&
tempo_utils::ConditionalLogMessage::operator=(ConditionalLogMessage &&other) noexcept
{
    if (this != &other) {
        m_filePath = other.m_filePath;
        m_lineNr = other.m_lineNr;
        m_severity = other.m_severity;
        m_category = other.m_category;
        m_enabled = other.m_enabled;
        m_ts = other.m_ts;
    }
    return *this;
}

tempo_utils::ConditionalLogMessage::~ConditionalLogMessage()
{
    if (m_enabled) {
        auto log = std::move(*buffer.rdbuf());
        write_log(m_ts, m_severity, m_filePath, m_lineNr, log.str());
        if (m_severity == LogSeverity::kFatal)
            std::abort();
    }
}

tempo_utils::LogVerbose::LogVerbose(char const *filePath, int lineNr, bool enabled)
    : ConditionalLogMessage(filePath, lineNr, LogSeverity::kVerbose, enabled)
{
}

tempo_utils::LogVerbose::~LogVerbose()
{
}

tempo_utils::LogVeryVerbose::LogVeryVerbose(char const *filePath, int lineNr, bool enabled)
    : ConditionalLogMessage(filePath, lineNr, LogSeverity::kVeryVerbose, enabled)
{
}

tempo_utils::LogVeryVerbose::~LogVeryVerbose()
{
}

tempo_utils::LogAssert::LogAssert(char const *assertCode, char const *filePath, int lineNr, bool enabled)
    : ConditionalLogMessage(filePath, lineNr, LogSeverity::kFatal, enabled)
{
    if (m_enabled) {
        buffer << "ASSERT FAILED: " << assertCode;
    }
}

tempo_utils::LogAssert::~LogAssert()
{
}