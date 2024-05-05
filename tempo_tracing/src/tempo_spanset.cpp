
#include <tempo_tracing/internal/spanset_reader.h>
#include <tempo_tracing/tempo_spanset.h>

tempo_tracing::TempoSpanset::TempoSpanset()
{
}

tempo_tracing::TempoSpanset::TempoSpanset(std::shared_ptr<const tempo_utils::ImmutableBytes> immutableBytes)
    : m_bytes(immutableBytes)
{
    TU_ASSERT (m_bytes != nullptr);
    std::span<const tu_uint8> bytes(m_bytes->getData(), m_bytes->getSize());
    m_reader = std::make_shared<const internal::SpansetReader>(bytes);
}

tempo_tracing::TempoSpanset::TempoSpanset(std::span<const tu_uint8> unownedBytes)
{
    m_reader = std::make_shared<const internal::SpansetReader>(unownedBytes);
}

tempo_tracing::TempoSpanset::TempoSpanset(const tempo_tracing::TempoSpanset &other)
    : m_bytes(other.m_bytes),
      m_reader(other.m_reader)
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

std::span<const tu_uint8>
tempo_tracing::TempoSpanset::bytesView() const
{
    if (!isValid())
        return {};
    return m_reader->bytesView();
}

/**
 * Verify that the given span of bytes is a valid spanset.
 *
 * @param bytes The span of bytes containing a spanset.
 * @param noIdentifier If true, then there is no identifier sequence preceding the data.
 * @return true if bytes refers to a valid spanset, otherwise false.
 */
bool
tempo_tracing::TempoSpanset::verify(std::span<const tu_uint8> bytes, bool noIdentifier)
{
    flatbuffers::Verifier verifier(bytes.data(), bytes.size());
    if (noIdentifier)
        return verifier.VerifyBuffer<tts1::Spanset>();
    return verifier.VerifyBuffer<tts1::Spanset>(tts1::SpansetIdentifier());
}
