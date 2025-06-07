#ifndef TEMPO_TRACING_SPANSET_STATE_H
#define TEMPO_TRACING_SPANSET_STATE_H

#include "tempo_spanset.h"
#include "trace_span.h"
#include "tracing_result.h"

namespace tempo_tracing {

    class SpansetState {

    public:
        SpansetState(tempo_utils::TraceId id);

        SpanData& appendSpan(
            const tempo_utils::SpanId &id,
            FailurePropagation propagation = FailurePropagation::NoPropagation,
            FailureCollection collection = FailureCollection::IgnoresPropagation);
        SpanData& appendSpan(
            const tempo_utils::SpanId &id,
            tu_uint32 parentIndex,
            const tempo_utils::SpanId &parentId,
            FailurePropagation propagation = FailurePropagation::NoPropagation,
            FailureCollection collection = FailureCollection::IgnoresPropagation);

        tempo_utils::Result<tempo_tracing::TempoSpanset> toSpanset() const;

    private:
        tempo_utils::TraceId m_id;
        std::list<SpanData> m_spans;
        uint32_t m_size;
    };
}

#endif // TEMPO_TRACING_SPANSET_STATE_H