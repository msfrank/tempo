// #ifndef TEMPO_TRACING_SCOPE_MANAGER_H
// #define TEMPO_TRACING_SCOPE_MANAGER_H
//
// #include <vector>
//
// #include "trace_recorder.h"
// #include "trace_span.h"
//
// namespace tempo_tracing {
//
//     enum class ScopeManagerState {
//         Inactive,
//         Pending,
//         Active,
//     };
//
//     class ScopeManager {
//
//     public:
//         explicit ScopeManager(std::shared_ptr<TraceRecorder> recorder);
//         ~ScopeManager();
//
//         std::shared_ptr<TraceRecorder> getRecorder() const;
//
//         ScopeManagerState getState() const;
//         void activate();
//         void deactivate();
//
//         std::shared_ptr<TraceSpan> makeSpan(
//             FailurePropagation propagation,
//             FailureCollection collection);
//         void pushSpan(std::shared_ptr<TraceSpan> span);
//         std::shared_ptr<TraceSpan> popSpan();
//         std::shared_ptr<TraceSpan> popSpanAndCheck(const tempo_utils::SpanId &spanId);
//         std::shared_ptr<TraceSpan> peekSpan() const;
//         std::shared_ptr<TraceSpan> peekSpanAndCheck(const tempo_utils::SpanId &spanId) const;
//         int stackSize() const;
//
//     private:
//         std::shared_ptr<TraceRecorder> m_recorder;
//         std::vector<std::shared_ptr<TraceSpan>> m_spanStack;
//         ScopeManagerState m_state;
//     };
// }
//
// #endif // TEMPO_TRACING_SCOPE_MANAGER_H