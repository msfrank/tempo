#ifndef TEMPO_TRACING_TEMPO_SPANSET_H
#define TEMPO_TRACING_TEMPO_SPANSET_H

#include "tracing_types.h"
#include "root_walker.h"
#include "error_walker.h"

namespace tempo_tracing {

    class TempoSpanset {

    public:
        TempoSpanset();
        explicit TempoSpanset(std::shared_ptr<const std::string> bytes);
        TempoSpanset(const TempoSpanset &other);

        bool isValid() const;

        SpansetVersion getABI() const;
        tempo_utils::TraceId getTraceId() const;
        RootWalker getRoots() const;
        ErrorWalker getErrors() const;

        std::shared_ptr<const internal::SpansetReader> getReader() const;
        std::shared_ptr<const std::string> getBytes() const;

    private:
        std::shared_ptr<const internal::SpansetReader> m_reader;
    };
}

#endif // TEMPO_TRACING_TEMPO_SPANSET_H
