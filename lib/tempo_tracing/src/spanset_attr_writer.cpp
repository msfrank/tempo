
#include <tempo_tracing/spanset_attr_writer.h>

tempo_tracing::SpansetAttrWriter::SpansetAttrWriter()
{
}

tempo_utils::Result<tu_uint32>
tempo_tracing::SpansetAttrWriter::putNamespace(const tempo_utils::Url &nsUrl)
{
    return 0;
}

tempo_utils::Result<tu_uint32>
tempo_tracing::SpansetAttrWriter::putNil()
{
    m_value = {};
    return 0;
}

tempo_utils::Result<tu_uint32>
tempo_tracing::SpansetAttrWriter::putBool(bool b)
{
    m_value = tempo_schema::AttrValue(b);
    return 0;
}

tempo_utils::Result<tu_uint32>
tempo_tracing::SpansetAttrWriter::putInt64(tu_int64 i64)
{
    m_value = tempo_schema::AttrValue(i64);
    return 0;
}

tempo_utils::Result<tu_uint32>
tempo_tracing::SpansetAttrWriter::putFloat64(double dbl)
{
    m_value = tempo_schema::AttrValue(dbl);
    return 0;
}

tempo_utils::Result<tu_uint32>
tempo_tracing::SpansetAttrWriter::putUInt64(tu_uint64 u64)
{
    m_value = tempo_schema::AttrValue(u64);
    return 0;
}

tempo_utils::Result<tu_uint32>
tempo_tracing::SpansetAttrWriter::putUInt32(tu_uint32 u32)
{
    m_value = tempo_schema::AttrValue(u32);
    return 0;
}

tempo_utils::Result<tu_uint32>
tempo_tracing::SpansetAttrWriter::putUInt16(tu_uint16 u16)
{
    m_value = tempo_schema::AttrValue(u16);
    return 0;
}

tempo_utils::Result<tu_uint32>
tempo_tracing::SpansetAttrWriter::putUInt8(tu_uint8 u8)
{
    m_value = tempo_schema::AttrValue(u8);
    return 0;
}

tempo_utils::Result<tu_uint32>
tempo_tracing::SpansetAttrWriter::putString(std::string_view str)
{
    m_value = tempo_schema::AttrValue(str);
    return 0;
}

tempo_schema::AttrValue
tempo_tracing::SpansetAttrWriter::getValue() const
{
    return m_value;
}
