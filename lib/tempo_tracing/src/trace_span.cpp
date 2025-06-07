
#include <tempo_tracing/active_scope.h>
#include <tempo_tracing/span_log.h>
#include <tempo_tracing/trace_span.h>
#include <tempo_tracing/trace_recorder.h>
#include <tempo_tracing/tracing_schema.h>
#include <tempo_tracing/tracing_types.h>
#include <tempo_utils/log_stream.h>

tempo_tracing::TraceSpan::TraceSpan(std::shared_ptr<tempo_tracing::TraceRecorder> recorder, SpanData &data)
    : m_recorder(recorder),
      m_data(data)
{
    TU_ASSERT (m_recorder != nullptr);
    m_lock = new absl::Mutex();
}

tempo_tracing::TraceSpan::~TraceSpan()
{
    close();
    delete m_lock;
}

tempo_utils::TraceId
tempo_tracing::TraceSpan::traceId() const
{
    // no lock is needed because span data will always be valid and trace id is immutable
    return m_recorder->traceId();
}

tempo_utils::SpanId
tempo_tracing::TraceSpan::spanId() const
{
    // no lock is needed because span data will always be valid and span id is immutable
    return m_data.spanId;
}

std::shared_ptr<tempo_tracing::TraceRecorder>
tempo_tracing::TraceSpan::traceRecorder() const
{
    // no lock is needed because span data will always be valid and span id is immutable
    return m_recorder;
}

std::string
tempo_tracing::TraceSpan::getOperationName() const
{
    absl::MutexLock locker(m_lock);
    return m_data.operationName;
}

void
tempo_tracing::TraceSpan::setOperationName(std::string_view name)
{
    absl::MutexLock locker(m_lock);
    TU_LOG_FATAL_IF(m_data.complete) << "failed to set operation name on closed span";
    m_data.operationName = name;
}

tempo_tracing::FailurePropagation
tempo_tracing::TraceSpan::getPropagation() const
{
    absl::MutexLock locker(m_lock);
    return m_data.propagation;
}

void
tempo_tracing::TraceSpan::setPropagation(FailurePropagation propagation)
{
    absl::MutexLock locker(m_lock);
    m_data.propagation = propagation;
}

tempo_tracing::FailureCollection
tempo_tracing::TraceSpan::getCollection() const
{
    absl::MutexLock locker(m_lock);
    return m_data.collection;
}

void
tempo_tracing::TraceSpan::setCollection(FailureCollection collection)
{
    absl::MutexLock locker(m_lock);
    m_data.collection = collection;
}

bool
tempo_tracing::TraceSpan::isFailed() const
{
    absl::MutexLock locker(m_lock);
    return m_data.failed;
}

void
tempo_tracing::TraceSpan::setFailed(bool failed)
{
    absl::MutexLock locker(m_lock);
    m_data.failed = true;
}

absl::Time
tempo_tracing::TraceSpan::getStartTime() const
{
    absl::MutexLock locker(m_lock);
    return m_data.startTime;
}

void
tempo_tracing::TraceSpan::setStartTime(absl::Time startTime)
{
    absl::MutexLock locker(m_lock);
    TU_LOG_FATAL_IF(m_data.complete) << "failed to set start time on closed span";
    if (m_data.startTime == absl::Time() || m_data.startTime > startTime) {
        m_data.startTime = startTime;
    }
}

absl::Time
tempo_tracing::TraceSpan::getEndTime() const
{
    absl::MutexLock locker(m_lock);
    return m_data.endTime;
}

void
tempo_tracing::TraceSpan::setEndTime(absl::Time endTime)
{
    absl::MutexLock locker(m_lock);
    TU_LOG_FATAL_IF(m_data.complete) << "failed to set end time on closed span";
    if (m_data.endTime == absl::Time() || m_data.endTime < endTime) {
        m_data.endTime = endTime;
    }
}

absl::Duration
tempo_tracing::TraceSpan::getActiveTime() const
{
    absl::MutexLock locker(m_lock);
    return m_data.activeTime;
}

void
tempo_tracing::TraceSpan::addActiveTime(absl::Duration duration)
{
    absl::MutexLock locker(m_lock);
    TU_LOG_FATAL_IF(m_data.complete) << "failed to set end time on closed span";
    m_data.activeTime += duration;
}

bool
tempo_tracing::TraceSpan::hasTag(const tempo_schema::AttrKey &key) const
{
    absl::MutexLock locker(m_lock);
    return m_data.tags.contains(key);
}

tempo_schema::AttrValue
tempo_tracing::TraceSpan::getTag(const tempo_schema::AttrKey &key) const
{
    absl::MutexLock locker(m_lock);
    if (!m_data.tags.contains(key))
        return {};
    return m_data.tags.at(key);
}

void
tempo_tracing::TraceSpan::putTagUnlocked(const tempo_schema::AttrKey &key, const tempo_schema::AttrValue &value)
{
    m_data.tags[key] = value;
}

tempo_tracing::LogEntry&
tempo_tracing::TraceSpan::appendLogUnlocked(absl::Time ts, LogSeverity severity)
{
    return m_data.logs.emplace_back(ts, severity, tempo_schema::AttrMap());
}

std::shared_ptr<tempo_tracing::SpanLog>
tempo_tracing::TraceSpan::appendLog(absl::Time ts, LogSeverity severity)
{
    absl::MutexLock locker(m_lock);
    TU_LOG_FATAL_IF(m_data.complete) << "failed to append log on closed span";
    auto &entry = appendLogUnlocked(absl::Now(), severity);
    switch (severity) {
        case LogSeverity::kFatal:
        case LogSeverity::kError:
            m_data.failed = true;
            break;
        default:
            break;
    }
    return std::make_shared<SpanLog>(shared_from_this(), entry);
}

void
tempo_tracing::TraceSpan::putFieldUnlocked(
    tempo_tracing::LogEntry &logEntry,
    const tempo_schema::AttrKey &key,
    const tempo_schema::AttrValue &value)
{
    logEntry.fields[key] = value;
}

std::shared_ptr<tempo_tracing::SpanLog>
tempo_tracing::TraceSpan::logMessage(std::string_view message, absl::Time ts, tempo_tracing::LogSeverity severity)
{
    auto log = appendLog(ts, severity);
    if (!message.empty()) {
        log->putField(tempo_tracing::kOpentracingMessage, std::string(message));
    }
    return log;
}

std::shared_ptr<tempo_tracing::SpanLog>
tempo_tracing::TraceSpan::logMessage(std::string_view message, tempo_tracing::LogSeverity severity)
{
    return logMessage(message, absl::Now(), severity);
}

std::shared_ptr<tempo_tracing::SpanLog>
tempo_tracing::TraceSpan::logStatus(const tempo_utils::Status &status, absl::Time ts, LogSeverity severity)
{
    auto log = appendLog(ts, severity);

    auto category = status.getErrorCategory();
    if (!category.empty()) {
        log->putField(tempo_tracing::kTempoTracingErrorCategoryName, std::string(category));
        log->putField(tempo_tracing::kTempoTracingErrorCode, static_cast<tu_int64>(status.getErrorCode()));
    }

    auto message = status.getMessage();
    if (!message.empty()) {
        log->putField(tempo_tracing::kOpentracingMessage, std::string(message));
    }

    return log;
}

std::shared_ptr<tempo_tracing::SpanLog>
tempo_tracing::TraceSpan::logStatus(const tempo_utils::Status &status, LogSeverity severity)
{
    return logStatus(status, absl::Now(), severity);
}

tempo_utils::Status
tempo_tracing::TraceSpan::checkStatus(const tempo_utils::Status &status, LogSeverity severity)
{
    if (status.notOk()) {
        logStatus(status, absl::Now(), severity);
    }
    return status;
}

bool
tempo_tracing::TraceSpan::isActive() const
{
    absl::MutexLock locker(m_lock);
    return m_scope != nullptr;
}

void
tempo_tracing::TraceSpan::activate(ActiveScope *scope)
{
    absl::MutexLock locker(m_lock);
    TU_ASSERT (m_scope == nullptr);
    m_scope = scope;
}

void
tempo_tracing::TraceSpan::deactivate()
{
    absl::MutexLock locker(m_lock);
    TU_ASSERT (m_scope != nullptr);
    m_scope = nullptr;
}

std::shared_ptr<tempo_tracing::TraceSpan>
tempo_tracing::TraceSpan::makeSpan(FailurePropagation propagation, FailureCollection collection)
{
    absl::MutexLock locker(m_lock);
    TU_LOG_FATAL_IF(m_data.complete) << "failed to make child span on closed span";
    return m_recorder->makeSpan(shared_from_this(), propagation, collection);
}

bool
tempo_tracing::TraceSpan::isOpen() const
{
    absl::MutexLock locker(m_lock);
    return !m_data.complete;
}

void
tempo_tracing::TraceSpan::close()
{
    absl::MutexLock locker(m_lock);
    if (!m_data.complete) {
        m_data.complete = true;
    }
}

void
tempo_tracing::TraceSpan::logStatusAndClose(
    std::string_view category,
    int code,
    LogSeverity severity,
    std::string_view message)
{
    absl::MutexLock locker(m_lock);
    TU_LOG_FATAL_IF(m_data.complete) << "failed to apply status to closed span";

    m_data.failed = true;
    m_data.complete = true;

    SpansetAttrWriter writer;

    tempo_tracing::kOpentracingEvent.writeAttr(&writer, "error");
    putTagUnlocked(tempo_tracing::kOpentracingEvent.getKey(), writer.getValue());

    auto &entry = appendLogUnlocked(absl::Now(), severity);

    if (!category.empty()) {
        tempo_tracing::kTempoTracingErrorCategoryName.writeAttr(&writer, std::string(category));
        putFieldUnlocked(entry, tempo_tracing::kTempoTracingErrorCategoryName.getKey(), writer.getValue());
        tempo_tracing::kTempoTracingErrorCode.writeAttr(&writer, code);
        putFieldUnlocked(entry, tempo_tracing::kTempoTracingErrorCode.getKey(), writer.getValue());
    }

    if (!message.empty()) {
        tempo_tracing::kOpentracingMessage.writeAttr(&writer, std::string(message));
        putFieldUnlocked(entry, tempo_tracing::kOpentracingMessage.getKey(), writer.getValue());
    }
}