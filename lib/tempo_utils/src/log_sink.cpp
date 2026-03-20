
#include <absl/strings/str_cat.h>
#include <boost/circular_buffer.hpp>

#include <tempo_utils/log_sink.h>
#include <tempo_utils/posix_result.h>

tempo_utils::DefaultLogSink::DefaultLogSink(bool displayShortForm, bool logToStdout)
    : m_displayShortForm(displayShortForm)
{
    m_sink = logToStdout? stdout : stderr;
}

tempo_utils::Status
tempo_utils::DefaultLogSink::openSink()
{
    return {};
}

void
tempo_utils::DefaultLogSink::writeLog(
    const absl::Time &ts,
    LogSeverity severity,
    const char *filePath,
    int lineNr,
    std::string_view message)
{
    if (m_displayShortForm) {
        std::fwrite(message.data(), message.size(), 1, m_sink);
        std::fwrite("\n", 1, 1, m_sink);
    }
    else {
        auto longForm = absl::StrCat(
            absl::FormatTime("%Y-%m-%d%ET%H:%M:%E6S%Ez", ts, absl::UTCTimeZone()),
            " ", log_severity_name(severity),
            " ", filePath, ":", lineNr,
            " ", message, "\n");
        std::fwrite(longForm.data(), longForm.size(), 1, m_sink);
    }
}

void
tempo_utils::DefaultLogSink::flushSink()
{
    std::fflush(m_sink);
}

void
tempo_utils::DefaultLogSink::closeSink()
{
    std::fflush(m_sink);
}

tempo_utils::LogFileSink::LogFileSink(
    const std::filesystem::path &logFilePath,
    bool displayShortForm)
    : m_logFilePath(logFilePath),
      m_displayShortForm(displayShortForm),
      m_sink(nullptr)
{
}

tempo_utils::LogFileSink::~LogFileSink()
{
    LogFileSink::closeSink();
}

tempo_utils::Status
tempo_utils::LogFileSink::openSink()
{
    m_sink = std::fopen(m_logFilePath.c_str(), "w+");
    if (m_sink == nullptr)
        return PosixStatus::last("failed to open log file");
    return {};
}

void
tempo_utils::LogFileSink::writeLog(
    const absl::Time &ts,
    LogSeverity severity,
    const char *filePath,
    int lineNr,
    std::string_view message)
{
    if (m_displayShortForm) {
        std::fwrite(message.data(), message.size(), 1, m_sink);
        std::fwrite("\n", 1, 1, m_sink);
    }
    else {
        auto longForm = absl::StrCat(
            absl::FormatTime("%Y-%m-%d%ET%H:%M:%E6S%Ez", ts, absl::UTCTimeZone()),
            " ", log_severity_name(severity),
            " ", filePath, ":", lineNr,
            " ", message, "\n");
        std::fwrite(longForm.data(), longForm.size(), 1, m_sink);
    }
}

void
tempo_utils::LogFileSink::flushSink()
{
    std::fflush(m_sink);
}

void
tempo_utils::LogFileSink::closeSink()
{
    if (m_sink) {
        std::fflush(m_sink);
        std::fclose(m_sink);
        m_sink = nullptr;
    }
}