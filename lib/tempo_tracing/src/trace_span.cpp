
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
    m_data.failed = failed;
}

absl::Time
tempo_tracing::TraceSpan::getStartTime() const
{
    absl::MutexLock locker(m_lock);
    return absl::FromUnixMillis(m_data.startTimeMillisSinceEpoch);
}

void
tempo_tracing::TraceSpan::setStartTime(absl::Time startTime)
{
    absl::MutexLock locker(m_lock);
    TU_LOG_FATAL_IF(m_data.complete) << "failed to set start time on closed span";
    m_data.startTimeMillisSinceEpoch = absl::ToUnixMillis(startTime);
}

absl::Time
tempo_tracing::TraceSpan::getEndTime() const
{
    absl::MutexLock locker(m_lock);
    return absl::FromUnixMillis(m_data.endTimeMillisSinceEpoch);
}

void
tempo_tracing::TraceSpan::setEndTime(absl::Time endTime)
{
    absl::MutexLock locker(m_lock);
    TU_LOG_FATAL_IF(m_data.complete) << "failed to set end time on closed span";
    m_data.endTimeMillisSinceEpoch = absl::ToUnixMillis(endTime);
}

bool
tempo_tracing::TraceSpan::isActive() const
{
    absl::MutexLock locker(m_lock);
    return m_data.activeTimeNanosSinceEpoch >= 0;
}

void
tempo_tracing::TraceSpan::activate()
{
    absl::MutexLock locker(m_lock);
    TU_LOG_FATAL_IF(m_data.complete) << "failed to activate closed span";
    auto now = absl::Now();
    // if start time has never been set then set it
    if (m_data.startTimeMillisSinceEpoch < 0) {
        m_data.startTimeMillisSinceEpoch = absl::ToUnixMillis(now);
    }
    // if we are not already active then set active time
    if (m_data.activeTimeNanosSinceEpoch < 0) {
        m_data.activeTimeNanosSinceEpoch = ToUnixNanos(now);
    }
}

void
tempo_tracing::TraceSpan::deactivateUnlocked()
{
    TU_LOG_FATAL_IF(m_data.complete) << "failed to deactivate closed span";
    // if we are not already active then do nothing
    if (m_data.activeTimeNanosSinceEpoch < 0)
        return;
    // calculate the active duration
    auto now = absl::Now();
    auto durationNanos = ToUnixNanos(now) - m_data.activeTimeNanosSinceEpoch;
    auto duration = absl::Nanoseconds(durationNanos);
    // add duration to existing duration
    m_data.activeDuration += duration;
    // clear the active time
    m_data.activeTimeNanosSinceEpoch = -1;
    // if end time has never been set then set it
    if (m_data.endTimeMillisSinceEpoch >= 0) {
        m_data.endTimeMillisSinceEpoch = absl::ToUnixMillis(now);
    }
}

void
tempo_tracing::TraceSpan::deactivate()
{
    absl::MutexLock locker(m_lock);
    deactivateUnlocked();
}

absl::Duration
tempo_tracing::TraceSpan::getActiveDuration() const
{
    absl::MutexLock locker(m_lock);
    return m_data.activeDuration;
}

void
tempo_tracing::TraceSpan::addToActiveDuration(absl::Duration duration)
{
    absl::MutexLock locker(m_lock);
    TU_LOG_FATAL_IF(m_data.complete) << "failed to add active time on closed span";
    m_data.activeDuration += duration;
}

void
tempo_tracing::TraceSpan::setActiveDuration(absl::Duration duration)
{
    absl::MutexLock locker(m_lock);
    TU_LOG_FATAL_IF(m_data.complete) << "failed to add active time on closed span";
    m_data.activeDuration = duration;
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
tempo_tracing::TraceSpan::logMessage(std::string_view message, absl::Time ts, LogSeverity severity)
{
    auto log = appendLog(ts, severity);
    if (!message.empty()) {
        log->putField(kOpentracingMessage, std::string(message));
    }
    return log;
}

std::shared_ptr<tempo_tracing::SpanLog>
tempo_tracing::TraceSpan::logMessage(std::string_view message, LogSeverity severity)
{
    return logMessage(message, absl::Now(), severity);
}

std::shared_ptr<tempo_tracing::SpanLog>
tempo_tracing::TraceSpan::logStatus(const tempo_utils::Status &status, absl::Time ts, LogSeverity severity)
{
    auto log = appendLog(ts, severity);

    auto category = status.getErrorCategory();
    if (!category.empty()) {
        log->putField(kTempoTracingErrorCategoryName, std::string(category));
        log->putField(kTempoTracingErrorCode, static_cast<tu_int64>(status.getErrorCode()));
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
        deactivateUnlocked();
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

    deactivateUnlocked();
    m_data.failed = true;
    m_data.complete = true;

    SpansetAttrWriter writer;

    kOpentracingEvent.writeAttr(&writer, "error");
    putTagUnlocked(kOpentracingEvent.getKey(), writer.getValue());

    auto &entry = appendLogUnlocked(absl::Now(), severity);

    if (!category.empty()) {
        kTempoTracingErrorCategoryName.writeAttr(&writer, std::string(category));
        putFieldUnlocked(entry, kTempoTracingErrorCategoryName.getKey(), writer.getValue());
        kTempoTracingErrorCode.writeAttr(&writer, code);
        putFieldUnlocked(entry, kTempoTracingErrorCode.getKey(), writer.getValue());
    }

    if (!message.empty()) {
        kOpentracingMessage.writeAttr(&writer, std::string(message));
        putFieldUnlocked(entry, kOpentracingMessage.getKey(), writer.getValue());
    }
}