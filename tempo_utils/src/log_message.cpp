#include <chrono>
#include <iostream>
#include <mutex>

#include <absl/time/clock.h>

#include <tempo_utils/logging.h>
#include <tempo_utils/log_message.h>

tempo_utils::LogMessage::LogMessage(char const *filePath, int lineNr, LogSeverity severity, bool enabled)
    : m_filePath(filePath),
      m_lineNr(lineNr),
      m_severity(severity),
      m_category(nullptr),
      m_enabled(enabled),
      m_ts(absl::Now())
{
}

tempo_utils::LogMessage::LogMessage(
    char const *filePath,
    int lineNr,
    LogSeverity severity,
    LogCategory const *category,
    bool enabled)
    : m_filePath(filePath),
      m_lineNr(lineNr),
      m_severity(severity),
      m_category(category),
      m_enabled(enabled),
      m_ts(absl::Now())
{
}

tempo_utils::LogMessage&
tempo_utils::LogMessage::operator=(LogMessage &&other) noexcept
{
    m_filePath = other.m_filePath;
    m_lineNr = other.m_lineNr;
    m_severity = other.m_severity;
    m_category = other.m_category;
    m_enabled = other.m_enabled;
    m_ts = other.m_ts;
    m_buffer = std::move(other.m_buffer);
    return *this;
}

tempo_utils::LogMessage::~LogMessage()
{
    if (m_enabled) {
        auto log = std::move(*m_buffer.rdbuf());
        write_log(m_ts, m_severity, m_filePath, m_lineNr, log.str());
        if (m_severity == LogSeverity::kFatal)
            std::abort();
    }
}

tempo_utils::LogVerbose::LogVerbose(char const *filePath, int lineNr, bool enabled)
    : LogMessage(filePath, lineNr, LogSeverity::kVerbose, enabled)
{
}

tempo_utils::LogVerbose::~LogVerbose()
{
}

tempo_utils::LogVeryVerbose::LogVeryVerbose(char const *filePath, int lineNr, bool enabled)
    : LogMessage(filePath, lineNr, LogSeverity::kVeryVerbose, enabled)
{
}

tempo_utils::LogVeryVerbose::~LogVeryVerbose()
{
}

tempo_utils::LogAssert::LogAssert(char const *assertCode, char const *filePath, int lineNr, bool enabled)
    : LogMessage(filePath, lineNr, LogSeverity::kFatal, enabled)
{
    if (m_enabled) {
        m_buffer << "ASSERT FAILED: " << assertCode;
    }
}

tempo_utils::LogAssert::~LogAssert()
{
}