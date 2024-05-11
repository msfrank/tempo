
#include <flatbuffers/idl.h>

#include <tempo_tracing/generated/spanset_schema.h>
#include <tempo_tracing/internal/spanset_reader.h>
#include <tempo_tracing/tracing_types.h>

tempo_tracing::internal::SpansetReader::SpansetReader(std::span<const tu_uint8> bytes)
    : m_bytes(bytes)
{
    m_spanset = tts1::GetSpanset(m_bytes.data());
}

bool
tempo_tracing::internal::SpansetReader::isValid() const
{
    return m_spanset != nullptr;
}

tts1::SpansetVersion
tempo_tracing::internal::SpansetReader::getABI() const
{
    if (m_spanset == nullptr)
        return tts1::SpansetVersion::Unknown;
    return m_spanset->abi();
}

tempo_utils::TraceId
tempo_tracing::internal::SpansetReader::getTraceId() const
{
    if (m_spanset == nullptr)
        return {};
    auto hi = m_spanset->trace_id_hi();
    auto lo = m_spanset->trace_id_lo();

    return tempo_utils::TraceId(hi, lo);
}

const tts1::NamespaceDescriptor *
tempo_tracing::internal::SpansetReader::getNamespace(uint32_t index) const
{
    if (m_spanset == nullptr)
        return nullptr;
    if (m_spanset->namespaces() && index < m_spanset->namespaces()->size())
        return m_spanset->namespaces()->Get(index);
    return nullptr;
}

uint32_t
tempo_tracing::internal::SpansetReader::numNamespaces() const
{
    if (m_spanset == nullptr)
        return 0;
    return m_spanset->namespaces()? m_spanset->namespaces()->size() : 0;
}

const tts1::AttributeDescriptor *
tempo_tracing::internal::SpansetReader::getAttribute(uint32_t index) const
{
    if (m_spanset == nullptr)
        return nullptr;
    if (m_spanset->attrs() && index < m_spanset->attrs()->size())
        return m_spanset->attrs()->Get(index);
    return nullptr;
}

uint32_t
tempo_tracing::internal::SpansetReader::numAttributes() const
{
    if (m_spanset == nullptr)
        return 0;
    return m_spanset->attrs()? m_spanset->attrs()->size() : 0;
}

const tts1::LogDescriptor *
tempo_tracing::internal::SpansetReader::getLog(uint32_t index) const
{
    if (m_spanset == nullptr)
        return nullptr;
    if (m_spanset->logs() && index < m_spanset->logs()->size())
        return m_spanset->logs()->Get(index);
    return nullptr;
}

uint32_t
tempo_tracing::internal::SpansetReader::numLogs() const
{
    if (m_spanset == nullptr)
        return 0;
    return m_spanset->logs()? m_spanset->logs()->size() : 0;
}

const tts1::SpanDescriptor *
tempo_tracing::internal::SpansetReader::getSpan(uint32_t index) const
{
    if (m_spanset == nullptr)
        return nullptr;
    if (m_spanset->spans() && index < m_spanset->spans()->size())
        return m_spanset->spans()->Get(index);
    return nullptr;
}

uint32_t
tempo_tracing::internal::SpansetReader::numSpans() const
{
    if (m_spanset == nullptr)
        return 0;
    return m_spanset->spans()? m_spanset->spans()->size() : 0;
}

uint32_t
tempo_tracing::internal::SpansetReader::getRoot(uint32_t index) const
{
    if (m_spanset == nullptr)
        return kInvalidAddressU32;
    if (m_spanset->roots() && index < m_spanset->roots()->size())
        return m_spanset->roots()->Get(index);
    return kInvalidAddressU32;
}

uint32_t
tempo_tracing::internal::SpansetReader::numRoots() const
{
    if (m_spanset == nullptr)
        return 0;
    return m_spanset->roots()? m_spanset->roots()->size() : 0;
}

uint32_t
tempo_tracing::internal::SpansetReader::getError(uint32_t index) const
{
    if (m_spanset == nullptr)
        return kInvalidAddressU32;
    if (m_spanset->errors() && index < m_spanset->errors()->size())
        return m_spanset->errors()->Get(index);
    return kInvalidAddressU32;
}

uint32_t
tempo_tracing::internal::SpansetReader::numErrors() const
{
    if (m_spanset == nullptr)
        return 0;
    return m_spanset->errors()? m_spanset->errors()->size() : 0;
}

std::span<const tu_uint8>
tempo_tracing::internal::SpansetReader::bytesView() const
{
    return m_bytes;
}

std::string
tempo_tracing::internal::SpansetReader::dumpJson() const
{
    flatbuffers::Parser parser;
    parser.Parse((const char *) tempo_tracing::schema::spanset::data);
    parser.opts.strict_json = true;

    std::string jsonData;
    auto *err = GenText(parser, m_bytes.data(), &jsonData);
    TU_ASSERT (err == nullptr);
    return jsonData;
}
