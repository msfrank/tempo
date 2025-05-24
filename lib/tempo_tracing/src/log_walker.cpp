
#include <tempo_tracing/internal/spanset_reader.h>
#include <tempo_tracing/log_walker.h>
#include <tempo_tracing/tracing_types.h>

tempo_tracing::LogWalker::LogWalker()
    : m_index(kInvalidAddressU32)
{
}

tempo_tracing::LogWalker::LogWalker(std::shared_ptr<const internal::SpansetReader> reader, tu_uint32 index)
    : m_reader(reader),
      m_index(index)
{
}

tempo_tracing::LogWalker::LogWalker(const LogWalker &other)
    : m_reader(other.m_reader),
      m_index(other.m_index)
{
}

bool
tempo_tracing::LogWalker::isValid() const
{
    return m_reader && m_reader->isValid() && m_index != kInvalidAddressU32;
}

absl::Time
tempo_tracing::LogWalker::getTimestamp() const
{
    if (!isValid())
        return {};
    auto *log = m_reader->getLog(m_index);
    TU_ASSERT (log != nullptr);
    return absl::FromUnixMillis(log->log_ts());
}

tempo_tracing::LogSeverity
tempo_tracing::LogWalker::getSeverity() const
{
    if (!isValid())
        return {};
    auto *log = m_reader->getLog(m_index);
    TU_ASSERT (log != nullptr);
    switch (log->log_severity()) {
        case tts1::LogSeverity::Fatal:          return LogSeverity::kFatal;
        case tts1::LogSeverity::Error:          return LogSeverity::kError;
        case tts1::LogSeverity::Warn:           return LogSeverity::kWarn;
        case tts1::LogSeverity::Info:           return LogSeverity::kInfo;
        case tts1::LogSeverity::Verbose:        return LogSeverity::kVerbose;
        case tts1::LogSeverity::VeryVerbose:    return LogSeverity::kVeryVerbose;
    }
    TU_UNREACHABLE();
}

bool
tempo_tracing::LogWalker::hasField(const tempo_schema::AttrKey &key) const
{
    auto index = findIndexForField(key);
    return index != kInvalidAddressU32;
}

bool
tempo_tracing::LogWalker::hasField(const tempo_schema::AbstractAttrValidator &validator) const
{
    return hasField(validator.getKey());
}

tu_uint32
tempo_tracing::LogWalker::findIndexForField(const tempo_schema::AttrKey &key) const
{
    if (!isValid())
        return kInvalidAddressU32;
    auto *log = m_reader->getLog(m_index);
    TU_ASSERT (log != nullptr);
    auto *fields = log->log_fields();
    if (fields == nullptr)    // log has no fields
        return kInvalidAddressU32;
    for (const auto fieldIndex : *fields) {
        auto *attr = m_reader->getAttribute(fieldIndex);
        TU_ASSERT (attr != nullptr);
        auto *ns = m_reader->getNamespace(attr->attr_ns());
        TU_ASSERT (ns != nullptr);
        auto *nsUrl = ns->ns_url();
        if (nsUrl == nullptr)
            continue;
        if (std::string_view(key.ns) == nsUrl->string_view() && key.id == attr->attr_type())
            return fieldIndex;
    }
    return kInvalidAddressU32;

}

int
tempo_tracing::LogWalker::numFields() const
{
    if (!isValid())
        return 0;
    auto *log = m_reader->getLog(m_index);
    TU_ASSERT (log != nullptr);
    auto *fields = log->log_fields();
    if (fields == nullptr)    // log has no fields
        return 0;
    return fields->size();
}
