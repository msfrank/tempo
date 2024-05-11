#ifndef TEMPO_TRACING_ERROR_WALKER_H
#define TEMPO_TRACING_ERROR_WALKER_H

#include "span_walker.h"

namespace tempo_tracing {

    class ErrorWalker {

    public:
        ErrorWalker();
        ErrorWalker(const ErrorWalker &other);

        bool isValid() const;

        SpanWalker getError(tu_uint32 index) const;
        int numErrors() const;

    private:
        std::shared_ptr<const internal::SpansetReader> m_reader;

        ErrorWalker(std::shared_ptr<const internal::SpansetReader> reader);
        friend class TempoSpanset;
    };
}


#endif // TEMPO_TRACING_ERROR_WALKER_H