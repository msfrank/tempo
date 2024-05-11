#ifndef TEMPO_UTILS_LOG_CONSOLE_H
#define TEMPO_UTILS_LOG_CONSOLE_H

#include "log_message.h"

#define TU_CONSOLE_OUT                  tempo_utils::LogStdout(true)
#define TU_CONSOLE_ERR                  tempo_utils::LogStderr(true)

#define TU_CONSOLE_OUT_IF(cond)         tempo_utils::LogStdout(cond? true : false)
#define TU_CONSOLE_ERR_IF(cond)         tempo_utils::LogStderr(cond? true : false)

namespace tempo_utils {

    class LogStdout : public LogMessage {
    public:
        LogStdout(bool enabled)
            : LogMessage(nullptr,
                         0,
                         enabled? LogSeverity::kConsoleStdout : LogSeverity::kFatal,
                         false) {};
        ~LogStdout();
    };

    class LogStderr : public LogMessage {
    public:
        LogStderr(bool enabled)
            : LogMessage(nullptr,
                         0,
                         enabled? LogSeverity::kConsoleStderr : LogSeverity::kFatal,
                         false) {};
        ~LogStderr();
    };
}

#endif // TEMPO_UTILS_LOG_CONSOLE_H
