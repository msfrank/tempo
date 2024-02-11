#ifndef TEMPO_TRACING_ROOT_WALKER_H
#define TEMPO_TRACING_ROOT_WALKER_H

#include "span_walker.h"
#include "tracing_types.h"

namespace tempo_tracing {

    class RootWalker {

    public:
        RootWalker();
        RootWalker(const RootWalker &other);

        bool isValid() const;

        SpanWalker getRoot(tu_uint32 index) const;
        int numRoots() const;

    private:
        std::shared_ptr<const internal::SpansetReader> m_reader;

        RootWalker(std::shared_ptr<const internal::SpansetReader> reader);
        friend class TempoSpanset;
    };
}

#endif // TEMPO_TRACING_ROOT_WALKER_H