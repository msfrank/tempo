#ifndef TEMPO_TRACING_TRACING_RESULT_H
#define TEMPO_TRACING_TRACING_RESULT_H

#include <fmt/core.h>
#include <fmt/format.h>

#include <tempo_utils/log_stream.h>
#include <tempo_utils/status.h>

#include "tracing_types.h"

namespace tempo_tracing {

    constexpr const char *kTempoTracingStatusNs = "dev.zuri.ns:tempo-tracing-status-1";

    enum class TracingCondition {
        kMissingTag,
        kMissingLog,
        kRecorderNotClosed,
        kTracingInvariant,
    };

    class TracingStatus : public tempo_utils::TypedStatus<TracingCondition> {
    public:
        using TypedStatus::TypedStatus;
        static TracingStatus ok();
        static bool convert(TracingStatus &dstStatus, const tempo_utils::Status &srcStatus);

    private:
        TracingStatus(tempo_utils::StatusCode statusCode, std::shared_ptr<const tempo_utils::Detail> detail);

    public:
        /**
          *
          * @param condition
          * @param message
          * @return
          */
        static TracingStatus forCondition(
            TracingCondition condition,
            std::string_view message)
        {
            return TracingStatus(condition, message);
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
        static TracingStatus forCondition(
            TracingCondition condition,
            fmt::string_view messageFmt = {},
            Args... messageArgs)
        {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            return TracingStatus(condition, message);
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
        static TracingStatus forCondition(
            TracingCondition condition,
            tempo_utils::TraceId traceId,
            tempo_utils::SpanId spanId,
            fmt::string_view messageFmt = {},
            Args... messageArgs)
        {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            return TracingStatus(condition, message, traceId, spanId);
        }
    };
}

namespace tempo_utils {

    template<>
    struct StatusTraits<tempo_tracing::TracingStatus> {
        using ConditionType = tempo_tracing::TracingCondition;
        static bool convert(tempo_tracing::TracingStatus &dstStatus, const tempo_utils::Status &srcStatus)
        {
            return tempo_tracing::TracingStatus::convert(dstStatus, srcStatus);
        }
    };

    template<>
    struct ConditionTraits<tempo_tracing::TracingCondition> {
        using StatusType = tempo_tracing::TracingStatus;
        static constexpr const char *condition_namespace() { return tempo_tracing::kTempoTracingStatusNs; }
        static constexpr StatusCode make_status_code(tempo_tracing::TracingCondition condition)
        {
            switch (condition) {
                case tempo_tracing::TracingCondition::kMissingTag:
                    return tempo_utils::StatusCode::kNotFound;
                case tempo_tracing::TracingCondition::kMissingLog:
                    return tempo_utils::StatusCode::kNotFound;
                case tempo_tracing::TracingCondition::kRecorderNotClosed:
                    return tempo_utils::StatusCode::kInternal;
                case tempo_tracing::TracingCondition::kTracingInvariant:
                    return tempo_utils::StatusCode::kInternal;
                default:
                    return tempo_utils::StatusCode::kUnknown;
            }
        };
        static constexpr const char *make_error_message(tempo_tracing::TracingCondition condition)
        {
            switch (condition) {
                case tempo_tracing::TracingCondition::kMissingTag:
                    return "Missing span tag";
                case tempo_tracing::TracingCondition::kMissingLog:
                    return "Missing log field";
                case tempo_tracing::TracingCondition::kRecorderNotClosed:
                    return "Recorder is not closed";
                case tempo_tracing::TracingCondition::kTracingInvariant:
                    return "Tracing invariant";
                default:
                    return "INVALID";
            }
        }
    };
}

#endif // TEMPO_TRACING_TRACING_RESULT_H