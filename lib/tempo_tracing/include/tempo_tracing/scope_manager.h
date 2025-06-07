#ifndef TEMPO_TRACING_SCOPE_MANAGER_H
#define TEMPO_TRACING_SCOPE_MANAGER_H

#include <stack>

#include "trace_recorder.h"
#include "trace_span.h"

namespace tempo_tracing {

    class ScopeManager {

    public:
        explicit ScopeManager(std::shared_ptr<TraceRecorder> recorder);
        ~ScopeManager();

        std::shared_ptr<TraceSpan> makeSpan(
            FailurePropagation propagation,
            FailureCollection collection);
        void pushSpan(std::shared_ptr<TraceSpan> span);
        std::shared_ptr<TraceSpan> popSpan();
        std::shared_ptr<TraceSpan> popSpanAndCheck(const tempo_utils::SpanId &spanId);
        std::shared_ptr<TraceSpan> peekSpan() const;
        int stackSize() const;

    private:
        std::shared_ptr<TraceRecorder> m_recorder;
        std::stack<std::shared_ptr<TraceSpan>> m_spanStack;
    };
}

#endif // TEMPO_TRACING_SCOPE_MANAGER_H