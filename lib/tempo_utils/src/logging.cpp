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
static std::FILE *loggingSink = stderr;

/**
 * Initialize logging with the given configuration.
 *
 * @param config The logging configuration.
 * @return true if initialization succeeds, otherwise false upon failure. As a special
 *   case, if initialization has already run, then this function does nothing and returns
 *   true.
 */
bool
tempo_utils::init_logging(const LoggingConfiguration &config)
{
    std::lock_guard lock(globalLock);
    loggingConfiguration = config;
    if (loggingConfiguration.logFile) {
        loggingSink = loggingConfiguration.logFile;
    } else if (loggingConfiguration.logToStdout) {
        loggingSink = stdout;
    } else {
        loggingSink = stderr;
    }
    return true;
}

/**
 * Return the current logging configuration.
 *
 * @return The logging configuration.
 */
tempo_utils::LoggingConfiguration
tempo_utils::get_logging_configuration()
{
    std::lock_guard lock(globalLock);
    return loggingConfiguration;
}

/**
 * Return a FILE pointer for the current logging sink.
 *
 * @return The FILE pointer. This should not be closed via fclose; use `cleanup_logging` instead.
 */
FILE *
tempo_utils::get_logging_sink()
{
    std::lock_guard lock(globalLock);
    return loggingSink;
}

/**
 * Clean up logging resources.
 *
 * @return true if cleanup succeeds, otherwise false upon failure. As a special case,
 *   if initialization has already run, then this function does nothing and returns true.
 */
bool
tempo_utils::cleanup_logging()
{
    return true;
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

/**
 * Write a log message.
 *
 * @param ts Timestamp when the log event was generated.
 * @param severity The severity of the log event.
 * @param filePath Path to the file where the log event was generated.
 * @param lineNr Line number in where the log event was generated.
 * @param log The log message.
 * @return true if the log was written to a sink, otherwise false.
 */
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
        std::fwrite(log.c_str(), log.size(), 1, loggingSink);
        std::fwrite("\n", 1, 1, loggingSink);
    }
    else {
        auto msg = absl::StrCat(
            absl::FormatTime("%Y-%m-%d%ET%H:%M:%E6S%Ez", ts, absl::UTCTimeZone()),
            " ", severityNames[static_cast<int>(severity)],
            " ", filePath, ":", lineNr,
            " ", log, "\n");
        std::fwrite(msg.c_str(), msg.size(), 1, loggingSink);
    }

    if (loggingConfiguration.flushEveryMessage) {
        std::fflush(loggingSink);
    }

    return true;
}

/**
 * Write log to the console.
 *
 * @param severity The severity of the log event.
 * @param log The log message.
 * @return true if the log was written to console, otherwise false.
 */
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
