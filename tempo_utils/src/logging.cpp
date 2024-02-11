#include <chrono>
#include <iostream>
#include <mutex>

#include <absl/strings/str_cat.h>
#include <absl/time/clock.h>

#include <tempo_utils/logging.h>

static const char *severityNames[9] = {
    "FATAL",
    "ERROR",
    "WARN",
    "INFO",
    "V",
    "VV",
    "STDOUT",
    "STDERR",
    nullptr,
};

// serialize logging through a global lock
static std::timed_mutex globalLock;
static tempo_utils::LoggingConfiguration loggingConfiguration = {
    tempo_utils::SeverityFilter::kDefault,
    false,
    false,
    false,
    false,
    nullptr,
};
static std::FILE *logSink = stderr;

bool
tempo_utils::init_logging(const LoggingConfiguration &config)
{
    std::lock_guard lock(globalLock);
    loggingConfiguration = config;
    if (loggingConfiguration.logFile) {
        logSink = loggingConfiguration.logFile;
    } else if (loggingConfiguration.logToStdout) {
        logSink = stdout;
    } else {
        logSink = stderr;
    }
    return true;
}

bool
tempo_utils::cleanup_logging()
{
    return false;
}

tempo_utils::LogCategory::LogCategory(char const *category)
    : m_category(category)
{
}

const char *
tempo_utils::LogCategory::category() const
{
    return m_category;
}

bool
tempo_utils::write_log(
    const absl::Time &ts,
    LogSeverity severity,
    const char *filePath,
    int lineNr,
    const std::string &log)
{
    std::chrono::duration<int, std::milli> timeout(100);
    std::unique_lock sink_lock(globalLock, timeout);
    if (!sink_lock.owns_lock())
        return false;

    switch (loggingConfiguration.severityFilter) {
        case tempo_utils::SeverityFilter::kSilent:
            if (severity != tempo_utils::LogSeverity::kFatal)
                return false;
            break;
        case tempo_utils::SeverityFilter::kErrorsOnly:
            if (severity > tempo_utils::LogSeverity::kError)
                return false;
            break;
        case tempo_utils::SeverityFilter::kWarningsAndErrors:
            if (severity > tempo_utils::LogSeverity::kWarn)
                return false;
            break;
        case tempo_utils::SeverityFilter::kDefault:
            if (severity > tempo_utils::LogSeverity::kInfo)
                return false;
            break;
        case tempo_utils::SeverityFilter::kVerbose:
            if (severity > tempo_utils::LogSeverity::kVerbose)
                return false;
            break;
        case tempo_utils::SeverityFilter::kVeryVerbose:
            break;
    }

    if (loggingConfiguration.displayShortForm) {
        std::fwrite(log.c_str(), log.size(), 1, logSink);
        std::fwrite("\n", 1, 1, logSink);
    }
    else {
        auto msg = absl::StrCat(
            absl::FormatTime(ts, absl::UTCTimeZone()),
            " ", severityNames[static_cast<int>(severity)],
            " ", filePath, ":", lineNr,
            " ", log, "\n");
        std::fwrite(msg.c_str(), msg.size(), 1, logSink);
    }

    if (loggingConfiguration.flushEveryMessage) {
        std::fflush(logSink);
    }

    return true;
}

bool
tempo_utils::write_console(LogSeverity severity, const std::string &log)
{
    std::chrono::duration<int, std::milli> timeout(100);
    std::unique_lock sink_lock(globalLock, timeout);
    if (!sink_lock.owns_lock())
        return false;

    if (loggingConfiguration.severityFilter == SeverityFilter::kSilent)
        return false;

    switch (severity) {
        case LogSeverity::kConsoleStdout: {
            std::cout << log << std::endl;
            return true;
        }
        case LogSeverity::kConsoleStderr: {
            std::cerr << log << std::endl;
            return true;
        }
        default:
            return false;
    }
}
