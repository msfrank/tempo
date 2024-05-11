
#include <tempo_tracing/active_scope.h>

tempo_tracing::ActiveScope::ActiveScope(std::shared_ptr<TraceSpan> span)
    : m_span(span)
{
    TU_ASSERT (m_span != nullptr);
    m_startTime = absl::Now();
    m_span->setStartTime(m_startTime);
}

tempo_tracing::ActiveScope::~ActiveScope()
{
    auto endTime = absl::Now();
    auto activeTime = endTime - m_startTime;
    m_span->setEndTime(endTime);
    m_span->addActiveTime(activeTime);
}