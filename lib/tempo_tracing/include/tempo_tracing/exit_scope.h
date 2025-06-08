#ifndef TEMPO_TRACING_EXIT_SCOPE_H
#define TEMPO_TRACING_EXIT_SCOPE_H

#include "base_scope.h"
#include "trace_context.h"
#include "trace_span.h"

namespace tempo_tracing {

    class ExitScope : public BaseScope {

    public:
        explicit ExitScope(std::string_view contextName = {});
        ~ExitScope();

        ExitScope(const ExitScope &) = delete;
        ExitScope(ExitScope &&) = delete;

        std::shared_ptr<TraceSpan> getSpan() const override;

    private:
        std::shared_ptr<TraceSpan> m_span;
    };
}

#endif // TEMPO_TRACING_EXIT_SCOPE_H
