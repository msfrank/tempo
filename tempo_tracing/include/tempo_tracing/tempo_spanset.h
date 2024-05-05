#ifndef TEMPO_TRACING_TEMPO_SPANSET_H
#define TEMPO_TRACING_TEMPO_SPANSET_H

#include <span>

#include <tempo_utils/immutable_bytes.h>

#include "tracing_types.h"
#include "root_walker.h"
#include "error_walker.h"

namespace tempo_tracing {

    class TempoSpanset {

    public:
        TempoSpanset();
        TempoSpanset(std::shared_ptr<const tempo_utils::ImmutableBytes> immutableBytes);
        TempoSpanset(std::span<const tu_uint8> unownedBytes);
        TempoSpanset(const TempoSpanset &other);

        bool isValid() const;

        SpansetVersion getABI() const;
        tempo_utils::TraceId getTraceId() const;
        RootWalker getRoots() const;
        ErrorWalker getErrors() const;

        std::shared_ptr<const internal::SpansetReader> getReader() const;
        std::span<const tu_uint8> bytesView() const;

        static bool verify(std::span<const tu_uint8> data, bool noIdentifier = false);

    private:
        std::shared_ptr<const tempo_utils::ImmutableBytes> m_bytes;
        std::shared_ptr<const internal::SpansetReader> m_reader;
    };
}

#endif // TEMPO_TRACING_TEMPO_SPANSET_H
