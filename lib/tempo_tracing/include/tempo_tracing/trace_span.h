#ifndef TEMPO_TRACING_TRACE_SPAN_H
#define TEMPO_TRACING_TRACE_SPAN_H

#include <absl/base/thread_annotations.h>
#include <fmt/core.h>
#include <fmt/format.h>

#include <tempo_utils/status.h>
#include <tempo_utils/tracing.h>

#include "tracing_result.h"
#include "tracing_types.h"
#include "spanset_attr_writer.h"
#include "tempo_schema/attr_serde.h"

namespace tempo_tracing {

    // forward declarations
    class ActiveScope;
    class SpanLog;
    class TraceRecorder;

    class TraceSpan : public std::enable_shared_from_this<TraceSpan> {

    public:
        ~TraceSpan();

        tempo_utils::TraceId traceId() const;
        tempo_utils::SpanId spanId() const;
        std::shared_ptr<TraceRecorder> traceRecorder() const;

        std::string getOperationName() const;
        void setOperationName(std::string_view name);

        absl::Time getStartTime() const;
        void setStartTime(absl::Time startTime);
        absl::Time getEndTime() const;
        void setEndTime(absl::Time endTime);
        absl::Duration getActiveTime() const;
        void addActiveTime(absl::Duration duration);

        bool hasTag(const tempo_schema::AttrKey &key) const;
        tempo_schema::AttrValue getTag(const tempo_schema::AttrKey &key) const;

        std::shared_ptr<SpanLog> appendLog(absl::Time ts, LogSeverity severity);
        std::shared_ptr<SpanLog> logMessage(std::string_view message, absl::Time ts, LogSeverity severity);
        std::shared_ptr<SpanLog> logMessage(std::string_view message, LogSeverity severity);
        std::shared_ptr<SpanLog> logStatus(const tempo_utils::Status &status, absl::Time ts, LogSeverity severity);
        std::shared_ptr<SpanLog> logStatus(const tempo_utils::Status &status, LogSeverity severity);

        tempo_utils::Status checkStatus(const tempo_utils::Status &status, LogSeverity severity = LogSeverity::kError);

        bool isActive() const;
        void activate(ActiveScope *scope);
        void deactivate();

        std::shared_ptr<TraceSpan> makeSpan();

        bool isOpen() const;
        void close();

    private:
        std::shared_ptr<TraceRecorder> m_recorder;
        absl::Mutex *m_lock;
        SpanData& m_data ABSL_GUARDED_BY(m_lock);
        ActiveScope *m_scope ABSL_GUARDED_BY(m_lock);

        TraceSpan(std::shared_ptr<TraceRecorder> recorder, SpanData &data);
        void putTagUnlocked(const tempo_schema::AttrKey &key, const tempo_schema::AttrValue &value);
        LogEntry& appendLogUnlocked(absl::Time ts, LogSeverity severity);
        void putFieldUnlocked(
            tempo_tracing::LogEntry &logEntry,
            const tempo_schema::AttrKey &key,
            const tempo_schema::AttrValue &value);
        void logStatusAndClose(std::string_view category, int code, LogSeverity severity, std::string_view message);

        friend class SpanLog;
        friend class TraceRecorder;

    public:
        /**
         *
         * @tparam T
         * @param serde
         * @param value
         * @return
         */
        template <typename T>
        tempo_utils::Status
        putTag(const tempo_schema::AttrSerde<T> &serde, const T &value)
        {
            SpansetAttrWriter writer;
            auto result = serde.writeAttr(&writer, value);
            if (result.isStatus())
                return result.getStatus();
            absl::MutexLock locker(m_lock);
            putTagUnlocked(serde.getKey(), writer.getValue());
            return TracingStatus::ok();
        }

        /**
         *
         */
        template <typename ResultType>
        tempo_utils::Result<ResultType>
        checkResult(tempo_utils::Result<ResultType> &&result, LogSeverity severity = LogSeverity::kError)
        {
            if (result.isStatus()) {
                logStatus(result.peekStatus(), absl::Now(), LogSeverity::kError);
            }
            return result;
        };

        /**
         *
         * @tparam Args
         * @param condition
         * @param messageFmt
         * @param messageArgs
         * @return
         */
        template <typename... Args>
        std::shared_ptr<SpanLog> logError(fmt::string_view messageFmt = {}, Args... messageArgs)
        {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            return logMessage(message, absl::Now(), LogSeverity::kError);
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
        std::shared_ptr<SpanLog> logWarn(fmt::string_view messageFmt = {}, Args... messageArgs)
        {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            return logMessage(message, absl::Now(), LogSeverity::kWarn);
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
        std::shared_ptr<SpanLog> logInfo(fmt::string_view messageFmt = {}, Args... messageArgs)
        {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            return logMessage(message, absl::Now(), LogSeverity::kInfo);
        }

        /**
         *
         * @tparam StatusType
         * @param condition
         * @param severity
         * @return
         */
        template<typename ConditionType,
            typename StatusType = typename tempo_utils::ConditionTraits<ConditionType>::StatusType>
        StatusType close(ConditionType condition, LogSeverity severity)
        {
            StatusType status(condition, {}, traceId(), spanId());
            logStatusAndClose(status.getErrorCategory(), status.getErrorCode(), severity, status.getMessage());
            return status;
        }

        /**
         *
         * @tparam Args
         * @param condition
         * @param messageFmt
         * @param messageArgs
         * @return
         */
        template<typename ConditionType,
            typename StatusType = typename tempo_utils::ConditionTraits<ConditionType>::StatusType,
            typename... Args>
        StatusType close(
            ConditionType condition,
            LogSeverity severity,
            fmt::string_view messageFmt,
            Args &&... messageArgs)
        {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            StatusType status(condition, message, traceId(), spanId());
            logStatusAndClose(status.getErrorCategory(), status.getErrorCode(), severity, status.getMessage());
            return status;
        }
    };
}

#endif // TEMPO_TRACING_TRACE_SPAN_H