#ifndef TEMPO_TRACING_LEAF_SCOPE_H
#define TEMPO_TRACING_LEAF_SCOPE_H

#include "base_scope.h"
#include "trace_context.h"
#include "trace_span.h"

namespace tempo_tracing {

    class LeafScope : public BaseScope {

    public:
        LeafScope(
            std::string_view operationName,
            std::string_view contextName = {},
            FailurePropagation propagation = FailurePropagation::NoPropagation,
            FailureCollection collection = FailureCollection::IgnoresPropagation);
        ~LeafScope();

        LeafScope() = delete;
        LeafScope(const LeafScope &) = delete;
        LeafScope(LeafScope &&) = delete;

        std::shared_ptr<TraceSpan> getSpan() const override;

    private:
        std::shared_ptr<TraceSpan> m_span;
    };
}

#endif // TEMPO_TRACING_LEAF_SCOPE_H
