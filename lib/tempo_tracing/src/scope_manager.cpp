//
// #include <tempo_tracing/scope_manager.h>
//
// tempo_tracing::ScopeManager::ScopeManager(std::shared_ptr<TraceRecorder> recorder)
//     : m_recorder(std::move(recorder)),
//       m_state(ScopeManagerState::Inactive)
// {
//     TU_ASSERT (m_recorder != nullptr);
// }
//
// tempo_tracing::ScopeManager::~ScopeManager()
// {
//     while (!m_spanStack.empty()) {
//         auto span = popSpan();
//         if (span->isOpen()) {
//             span->close();
//         }
//     }
// }
//
// std::shared_ptr<tempo_tracing::TraceRecorder>
// tempo_tracing::ScopeManager::getRecorder() const
// {
//     return m_recorder;
// }
//
// tempo_tracing::ScopeManagerState
// tempo_tracing::ScopeManager::getState() const
// {
//     return m_state;
// }
//
// void
// tempo_tracing::ScopeManager::activate()
// {
//     auto prevState = m_state;
//     if (prevState == ScopeManagerState::Active)
//         return;
//     m_state = ScopeManagerState::Active;
//     if (prevState == ScopeManagerState::Active) {
//         for (auto &span : std::reverse(m_spanStack.begin(), m_spanStack.end())) {
//             span->deactivate();
//         }
//     }
// }
//
// void
// tempo_tracing::ScopeManager::deactivate()
// {
//     auto prevState = m_state;
//     if (prevState == ScopeManagerState::Inactive)
//         return;
//     m_state = ScopeManagerState::Inactive;
//     if (prevState == ScopeManagerState::Active) {
//         for (auto &span : std::reverse(m_spanStack.begin(), m_spanStack.end())) {
//             span->deactivate();
//         }
//     }
// }
//
// std::shared_ptr<tempo_tracing::TraceSpan>
// tempo_tracing::ScopeManager::makeSpan(FailurePropagation propagation, FailureCollection collection)
// {
//     std::shared_ptr<TraceSpan> span;
//     if (!m_spanStack.empty()) {
//         auto &parent = m_spanStack.back();
//         span = parent->makeSpan(propagation, collection);
//     } else {
//         span = m_recorder->makeSpan(propagation, collection);
//     }
//     pushSpan(span);
//     return span;
// }
//
// void
// tempo_tracing::ScopeManager::pushSpan(std::shared_ptr<TraceSpan> span)
// {
//     m_spanStack.push_back(std::move(span));
// }
//
// std::shared_ptr<tempo_tracing::TraceSpan>
// tempo_tracing::ScopeManager::popSpan()
// {
//     if (m_spanStack.empty())
//         throw tempo_utils::StatusException(
//             TracingStatus::forCondition(TracingCondition::kTracingInvariant,
//                 "scope stack is empty"));
//     auto span = m_spanStack.back();
//     m_spanStack.pop_back();
//     return span;
// }
//
// std::shared_ptr<tempo_tracing::TraceSpan>
// tempo_tracing::ScopeManager::popSpanAndCheck(const tempo_utils::SpanId &spanId)
// {
//     if (m_spanStack.empty())
//         throw tempo_utils::StatusException(
//             TracingStatus::forCondition(TracingCondition::kTracingInvariant,
//                 "scope stack is empty"));
//     auto span = m_spanStack.back();
//     if (span->spanId() != spanId)
//         throw tempo_utils::StatusException(
//             TracingStatus::forCondition(TracingCondition::kTracingInvariant,
//                 "unexpected span on top of scope stack"));
//     m_spanStack.pop_back();
//     return span;
// }
//
// std::shared_ptr<tempo_tracing::TraceSpan>
// tempo_tracing::ScopeManager::peekSpan() const
// {
//     if (m_spanStack.empty())
//         throw tempo_utils::StatusException(
//             TracingStatus::forCondition(TracingCondition::kTracingInvariant,
//                 "scope stack is empty"));
//     return m_spanStack.back();
// }
//
// std::shared_ptr<tempo_tracing::TraceSpan>
// tempo_tracing::ScopeManager::peekSpanAndCheck(const tempo_utils::SpanId &spanId) const
// {
//     if (m_spanStack.empty())
//         throw tempo_utils::StatusException(
//             TracingStatus::forCondition(TracingCondition::kTracingInvariant,
//                 "scope stack is empty"));
//     const auto span = m_spanStack.back();
//     if (span->spanId() != spanId)
//         throw tempo_utils::StatusException(
//             TracingStatus::forCondition(TracingCondition::kTracingInvariant,
//                 "unexpected span on top of scope stack"));
//     return span;
// }
//
// int
// tempo_tracing::ScopeManager::stackSize() const
// {
//     return m_spanStack.size();
// }