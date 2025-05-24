#ifndef TEMPO_UTILS_STATUS_H
#define TEMPO_UTILS_STATUS_H

#include <string>

#include <fmt/core.h>
#include <fmt/format.h>

#include "integer_types.h"
#include "log_message.h"
#include "tracing.h"

namespace tempo_utils {

    template <typename ConditionType>
    struct ConditionTraits {};

    template <typename StatusType>
    struct StatusTraits {};

    enum class StatusCode {
        kOk,
        kCancelled,
        kInvalidArgument,
        kDeadlineExceeded,
        kNotFound,
        kAlreadyExists,
        kPermissionDenied,
        kUnauthenticated,
        kResourceExhausted,
        kFailedPrecondition,
        kAborted,
        kUnavailable,
        kOutOfRange,
        kUnimplemented,
        kInternal,
        kUnknown,
        NUM_CONDITIONS, // must be last
    };

    const char *status_code_to_string(StatusCode statusCode);

    class Detail {

    public:
        Detail(const char *errorCategory, int errorCode, std::string_view message, TraceId traceId, SpanId spanId);
        const char *getErrorCategory() const;
        int getErrorCode() const;
        std::string_view getMessage() const;
        TraceId getTraceId() const;
        SpanId getSpanId() const;
        std::string toString() const;

    private:
        const char *m_errorCategory;
        int m_errorCode;
        std::string m_message;
        TraceId m_traceId;
        SpanId m_spanId;
    };

    class Status {

    public:
        Status();
        Status(StatusCode statusCode, std::string_view message = {});
        Status(const Status &other);
        Status(Status &&other) noexcept;

        Status& operator=(const Status &other);
        Status& operator=(Status &&other) noexcept;

        //
        explicit operator bool() const { return m_statusCode == StatusCode::kOk; };

        bool isOk() const;
        bool notOk() const;

        bool isRetryable() const;

        StatusCode getStatusCode() const;
        std::shared_ptr<const Detail> getDetail() const;

        std::string_view getErrorCategory() const;
        int getErrorCode() const;
        std::string_view getMessage() const;

        std::string toString() const;

        bool isTraced() const;
        TraceId getTraceId() const;
        SpanId getSpanId() const;

        void andThrow() const;
        void orThrow() const;

    protected:
        Status(StatusCode statusCode, std::shared_ptr<const Detail> detail);
        Status(
            StatusCode statusCode,
            const char *errorCategory,
            int errorCode,
            std::string_view message,
            TraceId traceId,
            SpanId spanId);

    private:
        StatusCode m_statusCode;
        std::shared_ptr<const Detail> m_detail;

    public:
        /**
         *
         * @tparam StatusType
         * @tparam ConditionType
         * @param status
         * @return
         */
        template <typename StatusType>
        bool convertTo(StatusType &status)
        {
            return StatusTraits<StatusType>::convert(status, *this);
        };

        template <typename ConditionType,
            typename StatusType = typename ConditionTraits<ConditionType>::StatusType>
        bool matchesCondition(ConditionType condition)
        {
            StatusType status;
            if (!convertTo(status))
                return false;
            return status.getCondition() == condition;
        };
    };

    tempo_utils::LogMessage&& operator<<(tempo_utils::LogMessage &&message, const Status &status);

    template <typename ConditionType>
    class TypedStatus : public Status {
    public:
        TypedStatus() : Status(StatusCode::kOk) {};

        explicit TypedStatus(ConditionType condition)
            : Status(
                ConditionTraits<ConditionType>::make_status_code(condition),
                ConditionTraits<ConditionType>::condition_namespace(),
                static_cast<int>(condition),
                ConditionTraits<ConditionType>::make_error_message(condition),
                TraceId(),
                SpanId())
        {
        };

        TypedStatus(ConditionType condition, std::string_view message)
            : Status(
                ConditionTraits<ConditionType>::make_status_code(condition),
                ConditionTraits<ConditionType>::condition_namespace(),
                static_cast<int>(condition),
                message,
                TraceId(),
                SpanId())
        {
        };

        TypedStatus(
            ConditionType condition,
            std::string_view message,
            const tempo_utils::TraceId &traceId,
            const tempo_utils::SpanId &spanId)
            : Status(
                ConditionTraits<ConditionType>::make_status_code(condition),
                ConditionTraits<ConditionType>::condition_namespace(),
                static_cast<int>(condition),
                message,
                traceId,
                spanId)
        {
        };

        ConditionType getCondition() const
        {
            return static_cast<ConditionType>(getErrorCode());
        };

    protected:
        TypedStatus(StatusCode statusCode, std::shared_ptr<const Detail> detail)
            : Status(statusCode, detail)
        {
        }
    };

    class StatusException : public std::exception {

    public:
        StatusException(Status status) noexcept;
        Status getStatus() const;
        const char* what() const noexcept override;
    private:
        Status m_status;
    };

    constexpr const char *kTempoUtilsGenericStatusNs = "dev.zuri.ns:tempo-utils-generic-status-1";

    enum class GenericCondition {
        kInternalViolation,
        kUnimplemented,
    };

    class GenericStatus : public TypedStatus<GenericCondition> {
    public:
        using TypedStatus::TypedStatus;
        static GenericStatus ok();
        static bool convert(GenericStatus &dstStatus, const Status &srcStatus);

    private:
        GenericStatus(StatusCode statusCode, std::shared_ptr<const Detail> detail);

    public:
        /**
         *
         * @tparam Args
         * @param condition
         * @param messageFmt
         * @param messageArgs
         * @return
         */
        template <typename... Args>
        static GenericStatus forCondition(
            GenericCondition condition,
            fmt::string_view messageFmt = {},
            Args... messageArgs)
        {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            return GenericStatus(condition, message);
        }
        /**
         *
         * @tparam Args
         * @param condition
         * @param messageFmt
         * @param messageArgs
         * @return
         */
        template <typename... Args>
        static GenericStatus forCondition(
            GenericCondition condition,
            tempo_utils::TraceId traceId,
            tempo_utils::SpanId spanId,
            fmt::string_view messageFmt = {},
            Args... messageArgs)
        {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            return GenericStatus(condition, message, traceId, spanId);
        }
    };

    template<>
    struct StatusTraits<GenericStatus> {
        using ConditionType = GenericCondition;
        static bool convert(GenericStatus &dstStatus, const Status &srcStatus)
        {
            return GenericStatus::convert(dstStatus, srcStatus);
        }
    };

    template<>
    struct ConditionTraits<GenericCondition> {
        using StatusType = GenericStatus;
        static constexpr const char *condition_namespace() { return kTempoUtilsGenericStatusNs; }
        static constexpr StatusCode make_status_code(GenericCondition condition)
        {
            switch (condition) {
                case GenericCondition::kInternalViolation:
                    return tempo_utils::StatusCode::kInternal;
                case GenericCondition::kUnimplemented:
                    return tempo_utils::StatusCode::kUnimplemented;
                default:
                    return tempo_utils::StatusCode::kUnknown;
            }
        };
        static constexpr const char *make_error_message(GenericCondition condition)
        {
            switch (condition) {
                case GenericCondition::kInternalViolation:
                    return "Internal violation";
                case GenericCondition::kUnimplemented:
                    return "Unimplemented";
                default:
                    return "INVALID";
            }
        }
    };

    Status unimplemented(std::string_view reason);

    Status internalViolation(std::string_view reason);
}

/**
 * Macro which expects a valid expression returning an object convertible to`tempo_utils::Status`. If the
 * status is not OK then return from the current function immediately, otherwise continue processing.
 */
#define TU_RETURN_IF_NOT_OK(expr)                                           \
    do {                                                                    \
        auto status__ = static_cast<tempo_utils::Status>(expr);             \
        if (status__.notOk())                                               \
            return status__;                                                \
    } while (0)

/**
 * Macro which expects a valid expression returning an object convertible to`tempo_utils::Status`. If the
 * status is not OK then raise a `tempo_utils::StatusException` containing the status, otherwise continue
 * processing.
 */
#define TU_RAISE_IF_NOT_OK(expr)                                            \
    do {                                                                    \
        auto status__ = static_cast<tempo_utils::Status>(expr);             \
        if (status__.notOk())                                               \
            throw tempo_utils::StatusException(status__);                   \
    } while (0)

/**
 * Macro which expects a valid expression returning an object deriving from`tempo_utils::Result`. If the
 * result contains a status then return the status immediately, otherwise continue processing.
 */
#define TU_RETURN_IF_STATUS(expr)                                           \
    do {                                                                    \
        auto result__ = expr;                                               \
        if (result__.isStatus())                                            \
            return static_cast<tempo_utils::Status>(result__.getStatus());  \
    } while (0)

/**
 * Macro which expects a valid expression returning an object deriving from `tempo_utils::Result`. If the
 * result contains a status then raise a `tempo_utils::StatusException` containing the status, otherwise
 * continue processing.
 */
#define TU_RAISE_IF_STATUS(expr)                                            \
    do {                                                                    \
        auto result__ = expr;                                               \
        if (result__.isStatus())                                            \
            throw tempo_utils::StatusException(result__.getStatus());       \
    } while (0)

/**
 * Macro which expects an assignee which must be a valid lvalue and a valid expression returning an object
 * deriving from`tempo_utils::Result`. If the result contains a status then return the status immediately,
 * otherwise assign the result value to assignee and continue processing.
 */
#define TU_ASSIGN_OR_RETURN(assignee, expr)                                 \
    do {                                                                    \
        auto result__ = expr;                                               \
        if (result__.isStatus())                                            \
            return static_cast<tempo_utils::Status>(result__.getStatus());  \
        assignee = result__.getResult();                                    \
    } while (0)

/**
 * Macro which expects an assignee which must be a valid lvalue and a valid expression returning an object
 * deriving from`tempo_utils::Result`. If the result contains a status then raise a `tempo_utils::StatusException`
 * containing the status, otherwise assign the result value to assignee and continue processing.
 */
#define TU_ASSIGN_OR_RAISE(assignee, expr)                                  \
    do {                                                                    \
        auto result__ = expr;                                               \
        if (result__.isStatus())                                            \
            throw tempo_utils::StatusException(result__.getStatus());       \
        assignee = result__.getResult();                                    \
    } while (0)

#endif // TEMPO_UTILS_STATUS_H