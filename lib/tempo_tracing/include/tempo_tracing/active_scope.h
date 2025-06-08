// #ifndef TEMPO_TRACING_ACTIVE_SCOPE_H
// #define TEMPO_TRACING_ACTIVE_SCOPE_H
//
// #include "trace_span.h"
//
// namespace tempo_tracing {
//
//     class ActiveScope {
//
//     public:
//         explicit ActiveScope(std::shared_ptr<TraceSpan> span);
//         ~ActiveScope();
//
//         ActiveScope() = delete;
//         ActiveScope(const ActiveScope &) = delete;
//         ActiveScope(ActiveScope &&) = delete;
//
//     private:
//         std::shared_ptr<TraceSpan> m_span;
//         absl::Time m_startTime;
//     };
// }
//
// #endif // TEMPO_TRACING_ACTIVE_SCOPE_H