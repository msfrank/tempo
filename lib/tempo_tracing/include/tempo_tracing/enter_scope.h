#ifndef TEMPO_TRACING_ENTER_SCOPE_H
#define TEMPO_TRACING_ENTER_SCOPE_H

#include "scope_manager.h"
#include "trace_span.h"

namespace tempo_tracing {

    class EnterScope {

    public:
        explicit EnterScope(ScopeManager *scopeManager);
        EnterScope(std::string_view operationName, ScopeManager *scopeManager);
        ~EnterScope();

        std::shared_ptr<TraceSpan> getSpan();

        EnterScope() = delete;
        EnterScope(const EnterScope &) = delete;
        EnterScope(EnterScope &&) = delete;

    private:
        ScopeManager *m_scopeManager;
        std::shared_ptr<TraceSpan> m_span;
        absl::Time m_startTime;
    };
}

#endif // TEMPO_TRACING_ENTER_SCOPE_H