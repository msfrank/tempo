
#include <tempo_tracing/internal/thread_context.h>
#include <tempo_tracing/trace_context.h>
#include <tempo_utils/uuid.h>

tempo_tracing::TraceContext *
tempo_tracing::TraceContext::currentContext()
{
    auto &thread_context = internal::get_thread_context();
    return thread_context.currentContext;
}

tempo_tracing::TraceContext *
tempo_tracing::TraceContext::getContext(std::string_view name)
{
    auto &thread_context = internal::get_thread_context();
    auto entry = thread_context.contexts.find(name);
    if (entry != thread_context.contexts.cend())
        return entry->second.get();
    return nullptr;
}

tempo_utils::Result<tempo_tracing::TraceContext *>
tempo_tracing::TraceContext::switchCurrent(std::string_view name)
{
    auto &thread_context = internal::get_thread_context();
    if (thread_context.currentName == name)
        return thread_context.currentContext;

    // get the next context
    auto entry = thread_context.contexts.find(name);
    if (entry == thread_context.contexts.cend())
        return TracingStatus::forCondition(TracingCondition::kTracingInvariant,
            "missing trace context {}", name);
    auto &nextContext = entry->second;

    // if there is an active context then deactivate it
    if (thread_context.currentContext != nullptr) {
        thread_context.currentContext->deactivate();
    }

    //
    auto nextContextPtr = nextContext.get();
    nextContextPtr->activate();

    thread_context.currentContext = nextContextPtr;
    thread_context.currentName = name;

    return nextContextPtr;
}

tempo_utils::Status
tempo_tracing::TraceContext::clearCurrent()
{
    auto &thread_context = internal::get_thread_context();
    // if there is no active context then there is nothing to do
    if (thread_context.currentContext == nullptr)
        return {};

    // otherwise deactivate the context and clear the current data
    thread_context.currentContext->deactivate();
    thread_context.currentContext = nullptr;
    thread_context.currentName = {};
    return {};
}

tempo_utils::Result<tempo_tracing::TraceContext *>
tempo_tracing::TraceContext::makeContext(std::string_view name)
{
    auto &thread_context = internal::get_thread_context();
    std::string contextName;
    if (name.empty()) {
        auto uuid = tempo_utils::UUID::randomUUID();
        contextName = uuid.toString();
    } else {
        contextName = name;
    }
    if (thread_context.contexts.contains(contextName))
        return TracingStatus::forCondition(TracingCondition::kTracingInvariant,
            "trace context {} already exists", contextName);
    auto recorder = TraceRecorder::create();
    auto context = std::make_unique<TraceContext>(contextName, std::move(recorder));
    auto *contextPtr = context.get();
    thread_context.contexts[contextName] = std::move(context);
    return contextPtr;
}

tempo_utils::Result<tempo_tracing::TraceContext *>
tempo_tracing::TraceContext::makeContextAndSwitch(std::string_view name)
{
    TraceContext *context;
    TU_ASSIGN_OR_RETURN (context, makeContext(name));
    return switchCurrent(context->m_name);
}

tempo_utils::Result<tempo_tracing::TempoSpanset>
tempo_tracing::TraceContext::finishContext(std::string_view name)
{
    auto &thread_context = internal::get_thread_context();
    auto entry = thread_context.contexts.find(name);
    if (entry == thread_context.contexts.cend())
        return TracingStatus::forCondition(TracingCondition::kTracingInvariant,
            "missing trace context {}", name);

    auto &context = entry->second;
    auto recorder = context->m_recorder;

    thread_context.contexts.erase(entry);
    recorder->close();
    return recorder->toSpanset();
}

tempo_tracing::TraceContext::TraceContext(std::string_view name, std::shared_ptr<TraceRecorder> recorder)
    : m_name(name),
      m_recorder(std::move(recorder)),
      m_isActive(false)
{
    TU_ASSERT (!m_name.empty());
    TU_ASSERT (m_recorder != nullptr);
}

tempo_tracing::TraceContext::~TraceContext()
{
    while (!m_spanStack.empty()) {
        auto span = popSpan();
        if (span->isOpen()) {
            span->close();
        }
    }
}

std::string
tempo_tracing::TraceContext::getName() const
{
    return m_name;
}

tempo_utils::TraceId
tempo_tracing::TraceContext::getTraceId() const
{
    return m_recorder->traceId();
}

bool
tempo_tracing::TraceContext::isActive() const
{
    return m_isActive;
}

void
tempo_tracing::TraceContext::activate()
{
    if (m_isActive)
        return;
    for (auto &span : m_spanStack) {
        span->activate();
    }
    m_isActive = true;
}

void
tempo_tracing::TraceContext::deactivate()
{
    if (!m_isActive)
        return;
    for (auto it = m_spanStack.rbegin(); it != m_spanStack.rend(); it++) {
        auto &span = *it;
        span->deactivate();
    }
    m_isActive = false;
}

std::shared_ptr<tempo_tracing::TraceSpan>
tempo_tracing::TraceContext::pushSpan(FailurePropagation propagation, FailureCollection collection)
{
    std::shared_ptr<TraceSpan> span;
    if (!m_spanStack.empty()) {
        auto &parent = m_spanStack.back();
        span = parent->makeSpan(propagation, collection);
    } else {
        span = m_recorder->makeSpan(propagation, collection);
    }
    m_spanStack.push_back(span);
    return span;
}

std::shared_ptr<tempo_tracing::TraceSpan>
tempo_tracing::TraceContext::popSpan()
{
    if (m_spanStack.empty())
        throw tempo_utils::StatusException(
            TracingStatus::forCondition(TracingCondition::kTracingInvariant,
                "scope stack is empty"));
    auto span = m_spanStack.back();
    m_spanStack.pop_back();
    return span;
}

std::shared_ptr<tempo_tracing::TraceSpan>
tempo_tracing::TraceContext::popSpanAndCheck(const tempo_utils::SpanId &spanId)
{
    if (m_spanStack.empty())
        throw tempo_utils::StatusException(
            TracingStatus::forCondition(TracingCondition::kTracingInvariant,
                "scope stack is empty"));
    auto span = m_spanStack.back();
    if (span->spanId() != spanId)
        throw tempo_utils::StatusException(
            TracingStatus::forCondition(TracingCondition::kTracingInvariant,
                "unexpected span on top of scope stack"));
    m_spanStack.pop_back();
    return span;
}

std::shared_ptr<tempo_tracing::TraceSpan>
tempo_tracing::TraceContext::peekSpan() const
{
    if (m_spanStack.empty())
        throw tempo_utils::StatusException(
            TracingStatus::forCondition(TracingCondition::kTracingInvariant,
                "scope stack is empty"));
    return m_spanStack.back();
}

std::shared_ptr<tempo_tracing::TraceSpan>
tempo_tracing::TraceContext::peekSpanAndCheck(const tempo_utils::SpanId &spanId) const
{
    if (m_spanStack.empty())
        throw tempo_utils::StatusException(
            TracingStatus::forCondition(TracingCondition::kTracingInvariant,
                "scope stack is empty"));
    const auto span = m_spanStack.back();
    if (span->spanId() != spanId)
        throw tempo_utils::StatusException(
            TracingStatus::forCondition(TracingCondition::kTracingInvariant,
                "unexpected span on top of scope stack"));
    return span;
}

bool
tempo_tracing::TraceContext::isEmpty() const
{
    return m_spanStack.empty();
}

std::shared_ptr<tempo_tracing::TraceSpan>
tempo_tracing::TraceContext::getSpan(int index) const
{
    if (0 <= index && index < m_spanStack.size())
        return m_spanStack.at(index);
    return {};
}

int
tempo_tracing::TraceContext::numSpans() const
{
    return m_spanStack.size();
}

tempo_utils::Result<tempo_tracing::TempoSpanset>
tempo_tracing::TraceContext::finish()
{
    m_recorder->close();
    return m_recorder->toSpanset();
}
