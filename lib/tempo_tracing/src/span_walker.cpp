
#include <tempo_tracing/internal/spanset_reader.h>
#include <tempo_tracing/span_walker.h>
#include <tempo_tracing/tracing_types.h>

tempo_tracing::SpanWalker::SpanWalker()
    : m_index(kInvalidAddressU32)
{
}

tempo_tracing::SpanWalker::SpanWalker(std::shared_ptr<const internal::SpansetReader> reader, tu_uint32 index)
    : m_reader(reader),
      m_index(index)
{
    TU_ASSERT (m_reader != nullptr);
}

tempo_tracing::SpanWalker::SpanWalker(const SpanWalker &other)
    : m_reader(other.m_reader),
      m_index(other.m_index)
{
}

bool
tempo_tracing::SpanWalker::isValid() const
{
    return m_reader && m_reader->isValid() && m_index != kInvalidAddressU32;
}

tempo_utils::SpanId
tempo_tracing::SpanWalker::getId() const
{
    if (!isValid())
        return {};
    auto *span = m_reader->getSpan(m_index);
    TU_ASSERT (span != nullptr);
    return tempo_utils::SpanId(span->span_id());
}

std::string
tempo_tracing::SpanWalker::getOperationName() const
{
    if (!isValid())
        return {};
    auto *span = m_reader->getSpan(m_index);
    TU_ASSERT (span != nullptr);
    auto *operationName = span->operation_name();
    TU_ASSERT (operationName != nullptr);
    return operationName->str();
}

absl::Time
tempo_tracing::SpanWalker::getStartTime() const
{
    if (!isValid())
        return {};
    auto *span = m_reader->getSpan(m_index);
    TU_ASSERT (span != nullptr);
    return absl::FromUnixMillis(span->span_start());
}

absl::Time
tempo_tracing::SpanWalker::getEndTime() const
{
    if (!isValid())
        return {};
    auto *span = m_reader->getSpan(m_index);
    TU_ASSERT (span != nullptr);
    return absl::FromUnixMillis(span->span_end());
}

bool
tempo_tracing::SpanWalker::hasTag(const tempo_schema::AttrKey &key) const
{
    auto index = findIndexForTag(key);
    return index != kInvalidAddressU32;
}

bool
tempo_tracing::SpanWalker::hasTag(const tempo_schema::AbstractAttrValidator &validator) const
{
    return hasTag(validator.getKey());
}

tu_uint32
tempo_tracing::SpanWalker::findIndexForTag(const tempo_schema::AttrKey &key) const
{
    if (!isValid())
        return kInvalidAddressU32;
    auto *span = m_reader->getSpan(m_index);
    TU_ASSERT (span != nullptr);
    auto *tags = span->span_tags();
    if (tags == nullptr)    // span has no tags
        return kInvalidAddressU32;
    for (const auto tagIndex : *tags) {
        auto *attr = m_reader->getAttribute(tagIndex);
        TU_ASSERT (attr != nullptr);
        auto *ns = m_reader->getNamespace(attr->attr_ns());
        TU_ASSERT (ns != nullptr);
        auto *nsUrl = ns->ns_url();
        if (nsUrl == nullptr)
            continue;
        if (std::string_view(key.ns) == nsUrl->string_view() && key.id == attr->attr_type())
            return tagIndex;
    }
    return kInvalidAddressU32;
}

int
tempo_tracing::SpanWalker::numTags() const
{
    if (!isValid())
        return 0;
    auto *span = m_reader->getSpan(m_index);
    TU_ASSERT (span != nullptr);
    auto *tags = span->span_tags();
    if (tags == nullptr)    // span has no tags
        return 0;
    return tags->size();
}

tempo_tracing::LogWalker
tempo_tracing::SpanWalker::getLog(tu_uint32 index) const
{
    if (!isValid())
        return {};
    auto *span = m_reader->getSpan(m_index);
    TU_ASSERT (span != nullptr);
    auto *logs = span->span_logs();
    if (logs == nullptr)    // span has no logs
        return {};
    if (logs->size() <= index)
        return {};
    return LogWalker(m_reader, logs->Get(index));
}

int
tempo_tracing::SpanWalker::numLogs() const
{
    if (!isValid())
        return 0;
    auto *span = m_reader->getSpan(m_index);
    TU_ASSERT (span != nullptr);
    auto *logs = span->span_logs();
    if (logs == nullptr)    // span has no logs
        return 0;
    return logs->size();
}

bool
tempo_tracing::SpanWalker::hasParent() const
{
    if (!isValid())
        return {};
    auto *span = m_reader->getSpan(m_index);
    TU_ASSERT (span != nullptr);
    auto parent = span->parent();
    return parent != tempo_tracing::kInvalidAddressU32;
}

tempo_tracing::SpanWalker
tempo_tracing::SpanWalker::getParent() const
{
    if (!isValid())
        return {};
    auto *span = m_reader->getSpan(m_index);
    TU_ASSERT (span != nullptr);
    auto parent = span->parent();
    if (parent == tempo_tracing::kInvalidAddressU32)    // span has no parent
        return {};
    return SpanWalker(m_reader, parent);
}

tempo_tracing::SpanWalker
tempo_tracing::SpanWalker::getChild(tu_uint32 index) const
{
    if (!isValid())
        return {};
    auto *span = m_reader->getSpan(m_index);
    TU_ASSERT (span != nullptr);
    auto *children = span->children();
    if (children == nullptr)    // span has no children
        return {};
    if (children->size() <= index)
        return {};
    return SpanWalker(m_reader, children->Get(index));
}

int
tempo_tracing::SpanWalker::numChildren() const
{
    if (!isValid())
        return 0;
    auto *span = m_reader->getSpan(m_index);
    TU_ASSERT (span != nullptr);
    auto *children = span->children();
    if (children == nullptr)    // span has no children
        return 0;
    return children->size();
}

tempo_tracing::SpanWalker
tempo_tracing::SpanWalker::getSpanAtOffset(tu_uint32 offset) const
{
    if (!isValid())
        return {};
    if (m_reader->numSpans() <= offset)
        return {};
    return SpanWalker(m_reader, offset);
}
