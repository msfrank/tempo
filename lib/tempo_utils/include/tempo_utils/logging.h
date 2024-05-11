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

    struct LogCategory {
        char const *m_category;
        LogCategory(char const *category);
        const char *category() const;
    };

    bool init_logging(const LoggingConfiguration &config);

    LoggingConfiguration get_logging_configuration();

    FILE *get_logging_sink();

    bool cleanup_logging();

    bool write_log(
        const absl::Time &ts,
        LogSeverity severity,
        const char *filePath,
        int lineNr,
        const std::string &log);

    bool write_console(LogSeverity severity, const std::string &log);
}

#endif // TEMPO_UTILS_LOGGING_H
