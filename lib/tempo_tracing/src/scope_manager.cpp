
#include <tempo_tracing/scope_manager.h>

tempo_tracing::ScopeManager::ScopeManager(std::shared_ptr<TraceRecorder> recorder)
    : m_recorder(std::move(recorder))
{
    TU_ASSERT (m_recorder != nullptr);
}

tempo_tracing::ScopeManager::~ScopeManager()
{
    while (!m_spanStack.empty()) {
        auto span = popSpan();
        if (span->isOpen()) {
            auto endTime = absl::Now();
            auto activeTime = endTime - span->getStartTime();
            span->setEndTime(endTime);
            span->addActiveTime(activeTime);
            span->close();
        }
    }
}

std::shared_ptr<tempo_tracing::TraceSpan>
tempo_tracing::ScopeManager::makeSpan()
{
    std::shared_ptr<TraceSpan> span;
    if (!m_spanStack.empty()) {
        auto parent = m_spanStack.top();
        span = parent->makeSpan();
    } else {
        span = m_recorder->makeSpan();
    }
    pushSpan(span);
    return span;
}

void
tempo_tracing::ScopeManager::pushSpan(std::shared_ptr<TraceSpan> span)
{
    m_spanStack.push(span);
}

std::shared_ptr<tempo_tracing::TraceSpan>
tempo_tracing::ScopeManager::popSpan()
{
    if (m_spanStack.empty())
        throw tempo_utils::StatusException(
            TracingStatus::forCondition(TracingCondition::kTracingInvariant,
                "scope stack is empty"));
    auto span = m_spanStack.top();
    m_spanStack.pop();
    return span;
}

std::shared_ptr<tempo_tracing::TraceSpan>
tempo_tracing::ScopeManager::popSpanAndCheck(const tempo_utils::SpanId &spanId)
{
    if (m_spanStack.empty())
        throw tempo_utils::StatusException(
            TracingStatus::forCondition(TracingCondition::kTracingInvariant,
                "scope stack is empty"));
    auto span = m_spanStack.top();
    if (span->spanId() != spanId)
        throw tempo_utils::StatusException(
            TracingStatus::forCondition(TracingCondition::kTracingInvariant,
                "unexpected span on top of scope stack"));
    m_spanStack.pop();
    return span;
}

std::shared_ptr<tempo_tracing::TraceSpan>
tempo_tracing::ScopeManager::peekSpan() const
{
    return m_spanStack.top();
}

int
tempo_tracing::ScopeManager::stackSize() const
{
    return m_spanStack.size();
}