#ifndef TEMPO_TRACING_SPANSET_STATE_H
#define TEMPO_TRACING_SPANSET_STATE_H

#include <absl/container/node_hash_map.h>

#include "tempo_spanset.h"
#include "trace_span.h"
#include "tracing_result.h"

namespace tempo_tracing {

    class SpansetState {

    public:
        SpansetState(tempo_utils::TraceId id);

        SpanData& appendSpan(tempo_utils::SpanId id);
        SpanData& appendSpan(tempo_utils::SpanId id, tu_uint32 parentIndex, tempo_utils::SpanId parentId);

        tempo_utils::Result<tempo_tracing::TempoSpanset> toSpanset() const;

    private:
        tempo_utils::TraceId m_id;
        std::forward_list<SpanData> m_spans;
        std::forward_list<SpanData>::iterator m_last;
        uint32_t m_size;
    };
}

#endif // TEMPO_TRACING_SPANSET_STATE_H