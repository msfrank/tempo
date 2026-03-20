#ifndef TEMPO_UTILS_LOGGING_H
#define TEMPO_UTILS_LOGGING_H

#include <filesystem>
#include <string>

#include <absl/time/time.h>

#include "status.h"

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

    constexpr const char *log_severity_name(LogSeverity severity) {
        switch (severity) {
            case LogSeverity::kFatal:           return "FATAL";
            case LogSeverity::kError:           return "ERROR";
            case LogSeverity::kWarn:            return "WARN";
            case LogSeverity::kInfo:            return "INFO";
            case LogSeverity::kVerbose:         return "V";
            case LogSeverity::kVeryVerbose:     return "VV";
            case LogSeverity::kConsoleStdout:   return "STDOUT";
            case LogSeverity::kConsoleStderr:   return "STDERR";
            default:                            return "???";
        }
    }

    class AbstractLogSink {
    public:
        virtual ~AbstractLogSink() = default;
        virtual Status openSink() = 0;
        virtual void writeLog(
            const absl::Time &ts,
            LogSeverity severity,
            const char *filePath,
            int lineNr,
            std::string_view message) = 0;
        virtual void flushSink() = 0;
        virtual void closeSink() = 0;
    };

    struct LoggingConfiguration {
        SeverityFilter severityFilter = SeverityFilter::kDefault;
        bool flushEveryMessage = false;
    };

    struct LogCategory {
        char const *m_category;
        LogCategory(char const *category);
        const char *category() const;
    };

    Status init_logging(
        const LoggingConfiguration &config,
        std::unique_ptr<AbstractLogSink> &&logSink);

    Status init_logging(
        const LoggingConfiguration &config,
        bool displayShortForm = false,
        bool logToStdout = false);

    LoggingConfiguration get_logging_configuration();

    bool write_log(
        const absl::Time &ts,
        LogSeverity severity,
        const char *filePath,
        int lineNr,
        std::string_view message);

    bool cleanup_logging(bool finished = true);

    bool write_console(LogSeverity severity, std::string_view message);
}

#endif // TEMPO_UTILS_LOGGING_H
