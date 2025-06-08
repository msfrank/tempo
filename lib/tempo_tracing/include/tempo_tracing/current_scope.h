#ifndef TEMPO_TRACING_CURRENT_SCOPE_H
#define TEMPO_TRACING_CURRENT_SCOPE_H

#include "base_scope.h"
#include "trace_context.h"
#include "trace_span.h"

namespace tempo_tracing {

    class CurrentScope : public BaseScope {

    public:
        explicit CurrentScope(std::string_view contextName = {});
        ~CurrentScope();

        CurrentScope(const CurrentScope &) = delete;
        CurrentScope(CurrentScope &&) = delete;

        std::shared_ptr<TraceSpan> getSpan() const override;

    private:
        std::shared_ptr<TraceSpan> m_span;
    };
}

#endif // TEMPO_TRACING_CURRENT_SCOPE_H
