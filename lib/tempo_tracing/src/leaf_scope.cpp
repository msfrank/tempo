
#include <tempo_tracing/leaf_scope.h>
#include <tempo_tracing/trace_context.h>

tempo_tracing::LeafScope::LeafScope(
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

tempo_tracing::LeafScope::~LeafScope()
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
tempo_tracing::LeafScope::getSpan() const
{
    return m_span;
}
