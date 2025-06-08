
#include <tempo_tracing/base_scope.h>

static tempo_utils::Status
get_context(std::string_view contextName, tempo_tracing::TraceContext **contextptr)
{
    TU_ASSERT (contextptr != nullptr);
    if (!contextName.empty()) {
        TU_ASSIGN_OR_RETURN (*contextptr, tempo_tracing::TraceContext::switchCurrent(contextName));
    } else {
        *contextptr = tempo_tracing::TraceContext::currentContext();
        if (*contextptr == nullptr)
            return tempo_tracing::TracingStatus::forCondition(
                tempo_tracing::TracingCondition::kTracingInvariant, "missing current context");
    }
    return {};
}

tempo_tracing::BaseScope::BaseScope(std::string_view contextName)
    : m_context(nullptr)
{
    m_status = get_context(contextName, &m_context);
}

bool
tempo_tracing::BaseScope::checkCurrentContext()
{
    if (m_context == nullptr)
        return false;
    auto *current = tempo_tracing::TraceContext::currentContext();
    return m_context == current;
}

bool
tempo_tracing::BaseScope::isValid() const
{
    return getSpan() != nullptr;
}

tempo_utils::Status
tempo_tracing::BaseScope::getStatus() const
{
    return m_status;
}

void
tempo_tracing::BaseScope::setOperationName(std::string_view name)
{
    auto span = getSpan();
    if (span) {
        span->setOperationName(name);
    }
}

void
tempo_tracing::BaseScope::setPropagation(FailurePropagation propagation)
{
    auto span = getSpan();
    if (span) {
        span->setPropagation(propagation);
    }
}

void
tempo_tracing::BaseScope::setCollection(FailureCollection collection)
{
    auto span = getSpan();
    if (span) {
        span->setCollection(collection);
    }
}

void
tempo_tracing::BaseScope::setFailed(bool failed)
{
    auto span = getSpan();
    if (span) {
        span->setFailed(failed);
    }
}

std::shared_ptr<tempo_tracing::SpanLog>
tempo_tracing::BaseScope::appendLog(absl::Time ts, LogSeverity severity)
{
    auto span = getSpan();
    if (span)
        return span->appendLog(ts, severity);
    return {};
}

std::shared_ptr<tempo_tracing::SpanLog>
tempo_tracing::BaseScope::logMessage(std::string_view message, absl::Time ts, LogSeverity severity)
{
    auto span = getSpan();
    if (span)
        return span->logMessage(message, ts, severity);
    return {};
}

std::shared_ptr<tempo_tracing::SpanLog>
tempo_tracing::BaseScope::logMessage(std::string_view message, LogSeverity severity)
{
    auto span = getSpan();
    if (span)
        return span->logMessage(message, severity);
    return {};
}

std::shared_ptr<tempo_tracing::SpanLog>
tempo_tracing::BaseScope::logStatus(const tempo_utils::Status &status, absl::Time ts, LogSeverity severity)
{
    auto span = getSpan();
    if (span)
        return span->logStatus(status, ts, severity);
    return {};
}

std::shared_ptr<tempo_tracing::SpanLog>
tempo_tracing::BaseScope::logStatus(const tempo_utils::Status &status, LogSeverity severity)
{
    auto span = getSpan();
    if (span)
        return span->logStatus(status, severity);
    return {};
}