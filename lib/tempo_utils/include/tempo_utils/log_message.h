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

#define TU_ASSERT(cond)                 do { bool enabled = (cond)? false : true; tempo_utils::Assert(#cond, __FILE__, __LINE__, enabled);\
                                          if (enabled) __builtin_abort();\
                                        } while (false)

// construct assert log message if the specified lvalue is equal to nullptr

#define TU_NOTNULL(lval)                do { bool enabled = (lval) == nullptr; tempo_utils::Assert(#lval " is non-null", __FILE__, __LINE__, enabled);\
                                          if (enabled) __builtin_abort();\
                                        } while (false)

// assert that control flow is unreachable

#define TU_UNREACHABLE()                do { tempo_utils::Assert("invoked unreachable code", __FILE__, __LINE__, true);\
                                          __builtin_unreachable();\
                                        } while (false)

namespace tempo_utils {

    /**
     * Base log message implementation which contains only the buffer.
     */
    struct LogMessage {
        std::ostringstream buffer;
        LogMessage() = default;
        LogMessage(LogMessage &&other) noexcept;
        LogMessage& operator=(LogMessage &&other) noexcept;
    };

    /**
     *
     */
    struct LogToString : LogMessage {
        explicit LogToString(std::string *output);
        LogToString(LogToString &&other) noexcept;
        ~LogToString();
        LogToString& operator=(LogToString &&other) noexcept;

    private:
        std::string *m_output;
    };

    /**
     * Conditional log message.
     */
    struct ConditionalLogMessage : LogMessage {
        ConditionalLogMessage(char const *filePath, int lineNr, LogSeverity severity, bool enabled);
        ConditionalLogMessage(char const *filePath, int lineNr, LogSeverity severity, LogCategory const *category, bool enabled);
        ~ConditionalLogMessage();
        ConditionalLogMessage(ConditionalLogMessage &&other) noexcept = default;
        ConditionalLogMessage& operator=(ConditionalLogMessage &&other) noexcept;

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
    struct LogFatal : ConditionalLogMessage {
        LogFatal(char const *filePath, int lineNr, bool enabled)
            : ConditionalLogMessage(filePath, lineNr, LogSeverity::kFatal, enabled) {}
    };

    struct LogError : ConditionalLogMessage {
        LogError(char const *filePath, int lineNr, bool enabled)
            : ConditionalLogMessage(filePath, lineNr, LogSeverity::kError, enabled) {}
    };

    struct LogWarn : ConditionalLogMessage {
        LogWarn(char const *filePath, int lineNr, bool enabled)
            : ConditionalLogMessage(filePath, lineNr, LogSeverity::kWarn, enabled) {}
    };

    struct LogInfo : ConditionalLogMessage {
        LogInfo(char const *filePath, int lineNr, bool enabled)
            : ConditionalLogMessage(filePath, lineNr, LogSeverity::kInfo, enabled) {}
    };

    struct LogVerbose : ConditionalLogMessage {
        LogVerbose(char const *filePath, int lineNr, bool enabled);
        ~LogVerbose();
    };

    struct LogVeryVerbose : ConditionalLogMessage {
        LogVeryVerbose(char const *filePath, int lineNr, bool enabled);
        ~LogVeryVerbose();
    };

    struct LogAssert : ConditionalLogMessage {
        LogAssert(char const *assertCode, char const *filePath, int lineNr, bool enabled);
        ~LogAssert();
    };

    /**
     *
     */
    struct DiscardMessage {
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