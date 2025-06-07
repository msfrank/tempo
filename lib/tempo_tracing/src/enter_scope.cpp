
#include <tempo_tracing/enter_scope.h>

tempo_tracing::EnterScope::EnterScope(
    ScopeManager *scopeManager,
    FailurePropagation propagation,
    FailureCollection collection)
    : m_scopeManager(scopeManager)
{
    TU_ASSERT (m_scopeManager != nullptr);
    m_span = m_scopeManager->makeSpan(propagation, collection);
    m_startTime = absl::Now();
    m_span->setStartTime(m_startTime);
}

tempo_tracing::EnterScope::EnterScope(
    std::string_view operationName,
    ScopeManager *scopeManager,
    FailurePropagation propagation,
    FailureCollection collection)
    : EnterScope(scopeManager, propagation, collection)
{
    m_span->setOperationName(operationName);
}

tempo_tracing::EnterScope::~EnterScope()
{
    if (m_span == nullptr)
        return;
    m_scopeManager->popSpanAndCheck(m_span->spanId());
    if (m_span->isOpen()) {
        auto endTime = absl::Now();
        auto activeTime = endTime - m_startTime;
        m_span->setEndTime(endTime);
        m_span->addActiveTime(activeTime);
        m_span->close();
    }
}

std::shared_ptr<tempo_tracing::TraceSpan>
tempo_tracing::EnterScope::getSpan()
{
    return m_span;
}
