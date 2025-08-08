
#include <chrono>
#include <iostream>
#include <mutex>

#include <tempo_utils/logging.h>
#include <tempo_utils/log_sink.h>

// serialize logging through a global lock
static std::timed_mutex globalLock;
static tempo_utils::LoggingConfiguration currentConfiguration = {
    tempo_utils::SeverityFilter::kDefault,
    false,
};
static std::unique_ptr<tempo_utils::AbstractLogSink> currentSink;

/**
 * Initialize logging with the given configuration and sink.
 *
 * @param config The logging configuration.
 * @param logSink The logging sink.
 * @return true if initialization succeeds, otherwise false upon failure. As a special
 *   case, if initialization has already run, then this function does nothing and returns
 *   true.
 */
bool
tempo_utils::init_logging(
    const LoggingConfiguration &config,
    std::unique_ptr<AbstractLogSink> &&logSink)
{
    std::lock_guard lock(globalLock);
    currentConfiguration = config;
    currentSink = std::move(logSink);
    return true;
}

/**
 * Initialize logging with the given configuration using a `DefaultLogSink` instance.
 *
 * @param config The logging configuration.
 * @param displayShortForm If true, then print each log message in condensed form.
 * @param logToStdout If true, then write logs to stdout instead of stderr.
 * @return true if initialization succeeds, otherwise false upon failure. As a special
 *   case, if initialization has already run, then this function does nothing and returns
 *   true.
 */
bool
tempo_utils::init_logging(
    const LoggingConfiguration &config,
    bool displayShortForm,
    bool logToStdout)
{
    return init_logging(config, std::make_unique<DefaultLogSink>(displayShortForm, logToStdout));
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
    return currentConfiguration;
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
    currentSink.reset();
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
 * @param message The log message.
 * @return true if the log was written to a sink, otherwise false.
 */
bool
tempo_utils::write_log(
    const absl::Time &ts,
    LogSeverity severity,
    const char *filePath,
    int lineNr,
    std::string_view message)
{
    std::chrono::duration<int, std::milli> timeout(100);
    std::unique_lock sink_lock(globalLock, timeout);
    if (!sink_lock.owns_lock())
        return false;

    // if init_logging() was not called yet then create a default sink
    if (!currentSink) [[unlikely]] {
        currentSink = std::make_unique<DefaultLogSink>();
        currentSink->openSink();
    }

    switch (currentConfiguration.severityFilter) {
        case SeverityFilter::kSilent:
            if (severity != LogSeverity::kFatal) return false;
            break;
        case SeverityFilter::kErrorsOnly:
            if (severity > LogSeverity::kError) return false;
            break;
        case SeverityFilter::kWarningsAndErrors:
            if (severity > LogSeverity::kWarn) return false;
            break;
        case SeverityFilter::kDefault:
            if (severity > LogSeverity::kInfo) return false;
            break;
        case SeverityFilter::kVerbose:
            if (severity > LogSeverity::kVerbose) return false;
            break;
        case SeverityFilter::kVeryVerbose:
            break;
    }

    // write log to sink, and flush if requested
    if (currentSink->writeLog(ts, severity, filePath, lineNr, message)) {
        if (currentConfiguration.flushEveryMessage) {
            currentSink->flushSink();
        }
    }

    return true;
}

/**
 * Write log to the console.
 *
 * @param severity The severity of the log event.
 * @param message The log message.
 * @return true if the log was written to console, otherwise false.
 */
bool
tempo_utils::write_console(LogSeverity severity, std::string_view message)
{
    std::chrono::duration<int, std::milli> timeout(100);
    std::unique_lock sink_lock(globalLock, timeout);
    if (!sink_lock.owns_lock())
        return false;

    if (currentConfiguration.severityFilter == SeverityFilter::kSilent)
        return false;

    switch (severity) {
        case LogSeverity::kConsoleStdout: {
            std::cout << message << std::endl;
            return true;
        }
        case LogSeverity::kConsoleStderr: {
            std::cerr << message << std::endl;
            return true;
        }
        default:
            return false;
    }
}