#ifndef TEMPO_TRACING_SPAN_LOG_H
#define TEMPO_TRACING_SPAN_LOG_H

#include "trace_span.h"
#include "tracing_types.h"
#include "spanset_attr_writer.h"

namespace tempo_tracing {

    class SpanLog {

    public:
        SpanLog(std::shared_ptr<TraceSpan> span, LogEntry &logEntry);

        absl::Time getStartTime() const;
        LogSeverity getSeverity() const;

        bool hasField(const tempo_utils::AttrKey &key) const;
        tempo_utils::AttrValue getField(const tempo_utils::AttrKey &key) const;

    private:
        std::shared_ptr<TraceSpan> m_span;
        LogEntry& m_logEntry;

        void putFieldLocked(const tempo_utils::AttrKey &key, const tempo_utils::AttrValue &value);

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
        putField(const tempo_utils::AttrSerde<T> &serde, const T &value)
        {
            SpansetAttrWriter writer;
            auto result = serde.writeAttr(&writer, value);
            if (result.isStatus())
                return result.getStatus();
            putFieldLocked(serde.getKey(), writer.getValue());
            return TracingStatus::ok();
        }
    };
}

#endif // TEMPO_TRACING_SPAN_LOG_H