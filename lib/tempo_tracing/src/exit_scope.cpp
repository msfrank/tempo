
#include <tempo_tracing/exit_scope.h>
#include <tempo_tracing/trace_context.h>

tempo_tracing::ExitScope::ExitScope(std::string_view contextName)
    : BaseScope(contextName)
{
    if (m_context != nullptr) {
        m_context->activate();
        m_span = m_context->peekSpan();
        if (m_span != nullptr) {
            m_span->activate();
        }
    }
}

tempo_tracing::ExitScope::~ExitScope()
{
    if (m_span == nullptr)
        return;
    if (!checkCurrentContext())
        return;
    auto top = m_context->peekSpan();
    if (top && top->spanId() == m_span->spanId()) {
        m_span->close();
        m_context->popSpan();
    }
}

std::shared_ptr<tempo_tracing::TraceSpan>
tempo_tracing::ExitScope::getSpan() const
{
    return m_span;
}
