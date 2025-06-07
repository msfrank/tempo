#ifndef TEMPO_TRACING_TRACE_RECORDER_H
#define TEMPO_TRACING_TRACE_RECORDER_H

#include <mutex>
#include <random>

#include <absl/container/node_hash_map.h>
#include <absl/strings/string_view.h>

#include "spanset_state.h"
#include "tracing_types.h"
#include "trace_span.h"

namespace tempo_tracing {

    class TraceRecorder : public std::enable_shared_from_this<TraceRecorder> {

    public:
        static std::shared_ptr<TraceRecorder> create();
        static std::shared_ptr<TraceRecorder> create(tempo_utils::TraceId traceId);
        ~TraceRecorder();

        tempo_utils::TraceId traceId() const;

        std::shared_ptr<TraceSpan> makeSpan(
            FailurePropagation propagation = FailurePropagation::NoPropagation,
            FailureCollection collection = FailureCollection::IgnoresPropagation);
        std::shared_ptr<TraceSpan> makeSpan(
            std::shared_ptr<TraceSpan> parentSpan,
            FailurePropagation propagation = FailurePropagation::NoPropagation,
            FailureCollection collection = FailureCollection::IgnoresPropagation);
        void close();
        tempo_utils::Result<TempoSpanset> toSpanset() const;

    private:
        tempo_utils::TraceId m_id;
        absl::Mutex *m_lock;
        std::unique_ptr<SpansetState> m_state ABSL_GUARDED_BY(m_lock);
        bool m_closed ABSL_GUARDED_BY(m_lock);

        TraceRecorder();
        TraceRecorder(tempo_utils::TraceId id);
    };
}

#endif // TEMPO_TRACING_TRACE_RECORDER_H
