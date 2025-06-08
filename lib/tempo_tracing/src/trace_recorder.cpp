
#include <absl/strings/escaping.h>

#include <tempo_tracing/trace_recorder.h>
#include <tempo_tracing/trace_span.h>

tempo_tracing::TraceRecorder::TraceRecorder()
    : m_id(tempo_utils::TraceId::generate()),
      m_closed(false)
{
    m_state = std::make_unique<SpansetState>(m_id);
    m_lock = new absl::Mutex();
}

tempo_tracing::TraceRecorder::TraceRecorder(tempo_utils::TraceId id)
    : m_id(id),
      m_closed(false)
{
    m_state = std::make_unique<SpansetState>(m_id);
    m_lock = new absl::Mutex();
}

tempo_tracing::TraceRecorder::~TraceRecorder()
{
    delete m_lock;
}

tempo_utils::TraceId
tempo_tracing::TraceRecorder::traceId() const
{
    return m_id;
}

std::shared_ptr<tempo_tracing::TraceSpan>
tempo_tracing::TraceRecorder::makeSpan(FailurePropagation propagation, FailureCollection collection)
{
    absl::MutexLock locker(m_lock);

    if (m_closed)
        return {};

    SpanData& data = m_state->appendSpan(tempo_utils::SpanId::generate(), propagation, collection);

    auto *span = new TraceSpan(shared_from_this(), data);
    return std::shared_ptr<TraceSpan>(span);
}

std::shared_ptr<tempo_tracing::TraceSpan>
tempo_tracing::TraceRecorder::makeSpan(
    std::shared_ptr<TraceSpan> parentSpan,
    FailurePropagation propagation,
    FailureCollection collection)
{
    TU_ASSERT (parentSpan != nullptr);

    absl::MutexLock locker(m_lock);

    if (m_closed)
        return {};

    SpanData& parentData = parentSpan->m_data;
    SpanData& data = m_state->appendSpan(tempo_utils::SpanId::generate(), parentData.spanIndex,
        parentData.spanId, propagation, collection);
    parentData.children.push_back(data.spanIndex);

    auto *span = new TraceSpan(shared_from_this(), data);
    return std::shared_ptr<TraceSpan>(span);
}

bool
tempo_tracing::TraceRecorder::isClosed() const
{
    absl::MutexLock locker(m_lock);
    return m_closed;
}

void
tempo_tracing::TraceRecorder::close()
{
    absl::MutexLock locker(m_lock);
    m_closed = true;
}

tempo_utils::Result<tempo_tracing::TempoSpanset>
tempo_tracing::TraceRecorder::toSpanset() const
{
    absl::MutexLock locker(m_lock);
    if (!m_closed)
        return TracingStatus::forCondition(TracingCondition::kRecorderNotClosed);
    return m_state->toSpanset();
}

std::shared_ptr<tempo_tracing::TraceRecorder>
tempo_tracing::TraceRecorder::create()
{
    return std::shared_ptr<TraceRecorder>(new TraceRecorder());
}

std::shared_ptr<tempo_tracing::TraceRecorder>
tempo_tracing::TraceRecorder::create(tempo_utils::TraceId traceId)
{
    return std::shared_ptr<TraceRecorder>(new TraceRecorder(traceId));
}
