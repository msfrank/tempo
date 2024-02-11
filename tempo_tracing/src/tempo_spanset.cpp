
#include <tempo_tracing/internal/spanset_reader.h>
#include <tempo_tracing/tempo_spanset.h>

tempo_tracing::TempoSpanset::TempoSpanset()
{
}

tempo_tracing::TempoSpanset::TempoSpanset(std::shared_ptr<const std::string> bytes)
{
    if (!bytes->empty()) {
        m_reader = std::make_shared<internal::SpansetReader>(bytes);
    }
}

tempo_tracing::TempoSpanset::TempoSpanset(const tempo_tracing::TempoSpanset &other)
    : m_reader(other.m_reader)
{
}

bool
tempo_tracing::TempoSpanset::isValid() const
{
    if (m_reader == nullptr)
        return false;
    return m_reader->isValid();
}

tempo_tracing::SpansetVersion
tempo_tracing::TempoSpanset::getABI() const
{
    if (m_reader == nullptr)
        return SpansetVersion::Unknown;
    switch (m_reader->getABI()) {
        case tts1::SpansetVersion::Version1:
            return SpansetVersion::Version1;
        case tts1::SpansetVersion::Unknown:
        default:
            return SpansetVersion::Unknown;
    }
}

tempo_utils::TraceId
tempo_tracing::TempoSpanset::getTraceId() const
{
    if (m_reader == nullptr)
        return {};
    return m_reader->getTraceId();
}

tempo_tracing::RootWalker
tempo_tracing::TempoSpanset::getRoots() const
{
    return RootWalker(m_reader);
}

tempo_tracing::ErrorWalker
tempo_tracing::TempoSpanset::getErrors() const
{
    return ErrorWalker(m_reader);
}

std::shared_ptr<const tempo_tracing::internal::SpansetReader>
tempo_tracing::TempoSpanset::getReader() const
{
    return m_reader;
}

std::shared_ptr<const std::string>
tempo_tracing::TempoSpanset::getBytes() const
{
    if (m_reader == nullptr)
        return {};
    return m_reader->getBytes();
}
