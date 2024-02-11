#ifndef TEMPO_UTILS_LOGGING_H
#define TEMPO_UTILS_LOGGING_H

#include <filesystem>
#include <string>

#include <absl/time/time.h>

namespace tempo_utils {

    enum class LogSeverity {
        kFatal          = 0,
        kError          = 1,
        kWarn           = 2,
        kInfo           = 3,
        kVerbose        = 4,
        kVeryVerbose    = 5,
        kConsoleStdout  = 6,
        kConsoleStderr  = 7,
    };

    enum class SeverityFilter {
        kSilent,
        kErrorsOnly,
        kWarningsAndErrors,
        kDefault,
        kVerbose,
        kVeryVerbose,
    };

    struct LoggingConfiguration {
        SeverityFilter severityFilter = SeverityFilter::kDefault;
        bool colorizeOutput = false;
        bool displayShortForm = false;
        bool logToStdout = false;
        bool flushEveryMessage = false;
        std::FILE *logFile = nullptr;
    };

    /**
     *
     */
    struct LogCategory {
        char const *m_category;
        LogCategory(char const *category);
        const char *category() const;
    };

    /**
     * @brief Initialize logging with the given configuration.
     * @return True if initialization succeeds, otherwise false upon failure. As a special
     *   case, if initialization has already run, then this function does nothing and returns
     *   true.
     */
    bool init_logging(const LoggingConfiguration &config);

    /**
     * @brief Clean up logging library.
     * @return True if cleanup succeeds, otherwise false upon failure. As a special case,
     *   if initialization has already run, then this function does nothing and returns true.
     */
    bool cleanup_logging();

    /**
     * @brief Write a log message.
     * @param ts Timestamp when the log event was generated.
     * @param severity The severity of the log event.
     * @param filePath Path to the file where the log event was generated.
     * @param lineNr Line number in where the log event was generated.
     * @param log The log message.
     * @return True if the log was written to a sink, otherwise false.
     */
    bool write_log(
        const absl::Time &ts,
        LogSeverity severity,
        const char *filePath,
        int lineNr,
        const std::string &log);

    /**
     * @brief Write log to the console.
     * @param severity The severity of the log event.
     * @param log The log message.
     * @return True if the log was written to console, otherwise false;
     */
    bool write_console(LogSeverity severity, const std::string &log);
}

#endif // TEMPO_UTILS_LOGGING_H
