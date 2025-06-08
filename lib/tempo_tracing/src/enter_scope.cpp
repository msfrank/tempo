
#include <tempo_tracing/enter_scope.h>
#include <tempo_tracing/trace_context.h>

tempo_tracing::EnterScope::EnterScope(
    std::string_view operationName,
    std::string_view contextName,
    FailurePropagation propagation,
    FailureCollection collection)
    : BaseScope(contextName)
{
    if (m_context != nullptr) {
        m_context->activate();
        m_span = m_context->pushSpan(propagation, collection);
        m_span->setOperationName(operationName);
        m_span->activate();
    }
}

tempo_tracing::EnterScope::~EnterScope()
{
    if (m_span == nullptr)
        return;
    if (!checkCurrentContext())
        return;
    auto top = m_context->peekSpan();
    if (top && top->spanId() == m_span->spanId()) {
        m_span->deactivate();
    }
}

std::shared_ptr<tempo_tracing::TraceSpan>
tempo_tracing::EnterScope::getSpan() const
{
    return m_span;
}
