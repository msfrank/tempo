#ifndef TEMPO_UTILS_LOG_SINK_H
#define TEMPO_UTILS_LOG_SINK_H

#include "logging.h"

namespace tempo_utils {

    /**
     *
     */
    class DefaultLogSink : public AbstractLogSink {
    public:
        explicit DefaultLogSink(bool displayShortForm = false, bool logToStdout = false);
        Status openSink() override;
        void writeLog(
            const absl::Time &ts,
            LogSeverity severity,
            const char *filePath,
            int lineNr,
            std::string_view message) override;
        void flushSink() override;
        void closeSink() override;
    private:
        bool m_displayShortForm;
        std::FILE *m_sink;
    };

    /**
     *
     */
    class LogFileSink : public AbstractLogSink {
    public:
        explicit LogFileSink(
            const std::filesystem::path &logFilePath,
            bool displayShortForm = false);
        ~LogFileSink() override;
        Status openSink() override;
        void writeLog(
            const absl::Time &ts,
            LogSeverity severity,
            const char *filePath,
            int lineNr,
            std::string_view message) override;
        void flushSink() override;
        void closeSink() override;
    private:
        std::filesystem::path m_logFilePath;
        bool m_displayShortForm;
        std::FILE *m_sink;
    };

}

#endif // TEMPO_UTILS_LOG_SINK_H