
#include <tempo_tracing/span_log.h>
#include <tempo_tracing/trace_span.h>

tempo_tracing::SpanLog::SpanLog(std::shared_ptr<TraceSpan> span, LogEntry &logEntry)
    : m_span(span),
      m_logEntry(logEntry)
{
}

absl::Time tempo_tracing::SpanLog::getStartTime() const
{
    return m_logEntry.ts;
}

tempo_tracing::LogSeverity
tempo_tracing::SpanLog::getSeverity() const
{
    return m_logEntry.severity;
}

bool
tempo_tracing::SpanLog::hasField(const tempo_schema::AttrKey &key) const
{
    absl::MutexLock locker(m_span->m_lock);
    return m_logEntry.fields.contains(key);
}

tempo_schema::AttrValue
tempo_tracing::SpanLog::getField(const tempo_schema::AttrKey &key) const
{
    absl::MutexLock locker(m_span->m_lock);
    if (!m_logEntry.fields.contains(key))
        return {};
    return m_logEntry.fields.at(key);
}

void
tempo_tracing::SpanLog::putFieldLocked(const tempo_schema::AttrKey &key, const tempo_schema::AttrValue &value)
{
    absl::MutexLock locker(m_span->m_lock);
    TU_LOG_FATAL_IF(m_span->m_data.complete) << "failed to put field on closed span";
    m_span->putFieldUnlocked(m_logEntry, key, value);
}
