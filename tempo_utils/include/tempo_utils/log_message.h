#ifndef TEMPO_UTILS_LOG_MESSAGE_H
#define TEMPO_UTILS_LOG_MESSAGE_H

#include <sstream>

#include <absl/time/time.h>

#include "logging.h"

// strip log messages with a numeric severity above the following value

#define TU_LOG_STRIP_SEVERITY           3

// if true, strip verbose and very-verbose log messages

#define TU_LOG_STRIP_VERBOSE            false

// if true, strip debug assertions
#define TU_LOG_STRIP_ASSERT             false

// construct basic log message with the specified severity

#define TU_LOG_FATAL                    tempo_utils::Fatal(__FILE__, __LINE__, true)
#define TU_LOG_ERROR                    tempo_utils::Error(__FILE__, __LINE__, true)
#define TU_LOG_WARN                     tempo_utils::Warn(__FILE__, __LINE__, true)
#define TU_LOG_INFO                     tempo_utils::Info(__FILE__, __LINE__, true)

// construct basic log message if the specified condition is true

#define TU_LOG_FATAL_IF(cond)           tempo_utils::Fatal(__FILE__, __LINE__, cond? true : false)
#define TU_LOG_ERROR_IF(cond)           tempo_utils::Error(__FILE__, __LINE__, cond? true : false)
#define TU_LOG_WARN_IF(cond)            tempo_utils::Warn(__FILE__, __LINE__, cond? true : false)
#define TU_LOG_INFO_IF(cond)            tempo_utils::Info(__FILE__, __LINE__, cond? true : false)

// construct verbose log message

#define TU_LOG_V                        tempo_utils::Verbose(__FILE__, __LINE__, true)
#define TU_LOG_VV                       tempo_utils::VeryVerbose(__FILE__, __LINE__, true)

// construct verbose log message if the specified condition is true

#define TU_LOG_V_IF(category, cond)     tempo_utils::Verbose(__FILE__, __LINE__, category, cond? true : false)
#define TU_LOG_VV_IF(category, cond)    tempo_utils::VeryVerbose(__FILE__, __LINE__, category, cond? true : false)

// construct assert log message if the specified condition is false

#define TU_ASSERT(cond)                 tempo_utils::Assert(#cond, __FILE__, __LINE__, cond? false : true)

// assert that control flow is unreachable

#define TU_UNREACHABLE()                do { tempo_utils::Assert("invoked unreachable code", __FILE__, __LINE__, true);\
                                          __builtin_unreachable();\
                                        } while (false)

namespace tempo_utils {

    /**
     *
     */
    class LogMessage {
    public:
        LogMessage(char const *filePath, int lineNr, LogSeverity severity, bool enabled);
        LogMessage(char const *filePath, int lineNr, LogSeverity severity, LogCategory const *category, bool enabled);
        LogMessage(LogMessage &&other) noexcept = default;
        ~LogMessage();
        LogMessage& operator=(LogMessage &&other) noexcept;

        std::ostringstream m_buffer;

    protected:
        char const *m_filePath;
        int m_lineNr;
        LogSeverity m_severity;
        LogCategory const *m_category;
        bool m_enabled;
        absl::Time m_ts;
    };

    /**
     *
     */
    struct LogFatal : public LogMessage {
        LogFatal(char const *filePath, int lineNr, bool enabled)
            : LogMessage(filePath, lineNr, LogSeverity::kFatal, enabled) {};
    };

    struct LogError : public LogMessage {
        LogError(char const *filePath, int lineNr, bool enabled)
            : LogMessage(filePath, lineNr, LogSeverity::kError, enabled) {};
    };

    struct LogWarn : public LogMessage {
        LogWarn(char const *filePath, int lineNr, bool enabled)
            : LogMessage(filePath, lineNr, LogSeverity::kWarn, enabled) {};
    };

    struct LogInfo : public LogMessage {
        LogInfo(char const *filePath, int lineNr, bool enabled)
            : LogMessage(filePath, lineNr, LogSeverity::kInfo, enabled) {};
    };

    class LogVerbose : public LogMessage {
    public:
        LogVerbose(char const *filePath, int lineNr, bool enabled);
        ~LogVerbose();
    };

    class LogVeryVerbose : public LogMessage {
    public:
        LogVeryVerbose(char const *filePath, int lineNr, bool enabled);
        ~LogVeryVerbose();
    };

    class LogAssert : public LogMessage {
    public:
        LogAssert(char const *assertCode, char const *filePath, int lineNr, bool enabled);
        ~LogAssert();
    };

    /**
     *
     */
    struct DiscardMessage {
    public:
        DiscardMessage([[maybe_unused]] char const *filePath,
                       [[maybe_unused]] const int lineNr,
                       [[maybe_unused]] bool enabled) {};
        DiscardMessage([[maybe_unused]] char const *filePath,
                       [[maybe_unused]] int lineNr,
                       [[maybe_unused]] LogCategory const *category,
                       [[maybe_unused]] bool enabled) {};
        DiscardMessage([[maybe_unused]] char const *assertCode,
                       [[maybe_unused]] char const *filePath,
                       [[maybe_unused]] const int lineNr,
                       [[maybe_unused]] bool enabled) {};
    };

    // internal macro to define bool type parameter for std::conditional
    #define SEVERITY_STRIP_CONDITION(severity) TU_LOG_STRIP_SEVERITY >= static_cast<int>(LogSeverity::severity)

    // conditionally define LogMessage classes based on the value of TU_LOG_STRIP_SEVERITY
    typedef std::conditional<SEVERITY_STRIP_CONDITION(kFatal),LogFatal,DiscardMessage>::type Fatal;
    typedef std::conditional<SEVERITY_STRIP_CONDITION(kError),LogError,DiscardMessage>::type Error;
    typedef std::conditional<SEVERITY_STRIP_CONDITION(kWarn),LogWarn,DiscardMessage>::type Warn;
    typedef std::conditional<SEVERITY_STRIP_CONDITION(kInfo),LogInfo,DiscardMessage>::type Info;

    // conditionally define LogMessage classes based on the value of TU_LOG_STRIP_VERBOSE
    typedef std::conditional<!TU_LOG_STRIP_VERBOSE,LogVerbose,DiscardMessage>::type Verbose;
    typedef std::conditional<!TU_LOG_STRIP_VERBOSE,LogVeryVerbose,DiscardMessage>::type VeryVerbose;

    // conditionally define LogMessage classes based on the value of TU_LOG_STRIP_ASSERT
    typedef std::conditional<!TU_LOG_STRIP_ASSERT,LogAssert,DiscardMessage>::type Assert;

    // compile away to nothing if streaming to DiscardMessage
    template<class T>
    inline DiscardMessage&& operator<<(DiscardMessage &&message, const T &)
    {
        return std::move(message);
    }
}

#endif // TEMPO_UTILS_LOG_MESSAGE_H