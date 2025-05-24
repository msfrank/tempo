
#include <tempo_schema/schema_result.h>
#include <tempo_tracing/internal/spanset_reader.h>
#include <tempo_tracing/spanset_attr_parser.h>

tempo_tracing::SpansetAttrParser::SpansetAttrParser(std::shared_ptr<const internal::SpansetReader> reader)
    : m_reader(reader)
{
    TU_ASSERT (m_reader != nullptr);
}

tempo_utils::Status
tempo_tracing::SpansetAttrParser::getNil(tu_uint32 index, std::nullptr_t &nil)
{
    if (!m_reader->isValid())
        return tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kParseError, "invalid reader");
    auto *attr = m_reader->getAttribute(index);
    TU_ASSERT (attr != nullptr);
    if (attr->attr_value_type() != tts1::Value::TrueFalseNilValue)
        return tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kWrongType,"attr type mismatch");
    auto *value = attr->attr_value_as_TrueFalseNilValue();
    if (value->tfn() != tts1::TrueFalseNil::Nil)
        return tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kWrongType,"attr type mismatch");
    nil = nullptr;
    return {};
}

tempo_utils::Status
tempo_tracing::SpansetAttrParser::getBool(tu_uint32 index, bool &b)
{
    if (!m_reader->isValid())
        return tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kParseError, "invalid reader");
    auto *attr = m_reader->getAttribute(index);
    TU_ASSERT (attr != nullptr);
    if (attr->attr_value_type() != tts1::Value::TrueFalseNilValue)
        return tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kWrongType,"attr type mismatch");
    auto *value = attr->attr_value_as_TrueFalseNilValue();
    switch (value->tfn()) {
        case tts1::TrueFalseNil::True:
            b = true;
            return {};
        case tts1::TrueFalseNil::False:
            b = false;
            return {};
        case tts1::TrueFalseNil::Nil:
        default:
            return tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kWrongType,"attr type mismatch");
    }
}

tempo_utils::Status
tempo_tracing::SpansetAttrParser::getInt64(tu_uint32 index, tu_int64 &i64)
{
    if (!m_reader->isValid())
        return tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kParseError, "invalid reader");
    auto *attr = m_reader->getAttribute(index);
    TU_ASSERT (attr != nullptr);
    if (attr->attr_value_type() != tts1::Value::Int64Value)
        return tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kWrongType,"attr type mismatch");
    auto *value = attr->attr_value_as_Int64Value();
    i64 = value->i64();
    return {};
}

tempo_utils::Status
tempo_tracing::SpansetAttrParser::getFloat64(tu_uint32 index, double &dbl)
{
    if (!m_reader->isValid())
        return tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kParseError, "invalid reader");
    auto *attr = m_reader->getAttribute(index);
    TU_ASSERT (attr != nullptr);
    if (attr->attr_value_type() != tts1::Value::Float64Value)
        return tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kWrongType,"attr type mismatch");
    auto *value = attr->attr_value_as_Float64Value();
    dbl = value->f64();
    return {};
}

tempo_utils::Status
tempo_tracing::SpansetAttrParser::getUInt64(tu_uint32 index, tu_uint64 &u64)
{
    if (!m_reader->isValid())
        return tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kParseError, "invalid reader");
    auto *attr = m_reader->getAttribute(index);
    TU_ASSERT (attr != nullptr);
    if (attr->attr_value_type() != tts1::Value::UInt64Value)
        return tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kWrongType,"attr type mismatch");
    auto *value = attr->attr_value_as_UInt64Value();
    u64 = value->u64();
    return {};
}

tempo_utils::Status
tempo_tracing::SpansetAttrParser::getUInt32(tu_uint32 index, tu_uint32 &u32)
{
    if (!m_reader->isValid())
        return tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kParseError, "invalid reader");
    auto *attr = m_reader->getAttribute(index);
    TU_ASSERT (attr != nullptr);
    if (attr->attr_value_type() != tts1::Value::UInt32Value)
        return tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kWrongType,"attr type mismatch");
    auto *value = attr->attr_value_as_UInt32Value();
    u32 = value->u32();
    return {};
}

tempo_utils::Status
tempo_tracing::SpansetAttrParser::getUInt16(tu_uint32 index, tu_uint16 &u16)
{
    if (!m_reader->isValid())
        return tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kParseError, "invalid reader");
    auto *attr = m_reader->getAttribute(index);
    TU_ASSERT (attr != nullptr);
    if (attr->attr_value_type() != tts1::Value::UInt16Value)
        return tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kWrongType,"attr type mismatch");
    auto *value = attr->attr_value_as_UInt16Value();
    u16 = value->u16();
    return {};
}

tempo_utils::Status
tempo_tracing::SpansetAttrParser::getUInt8(tu_uint32 index, tu_uint8 &u8)
{
    if (!m_reader->isValid())
        return tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kParseError, "invalid reader");
    auto *attr = m_reader->getAttribute(index);
    TU_ASSERT (attr != nullptr);
    if (attr->attr_value_type() != tts1::Value::UInt8Value)
        return tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kWrongType,"attr type mismatch");
    auto *value = attr->attr_value_as_UInt8Value();
    u8 = value->u8();
    return {};
}

tempo_utils::Status
tempo_tracing::SpansetAttrParser::getString(tu_uint32 index, std::string &str)
{
    if (!m_reader->isValid())
        return tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kParseError, "invalid reader");
    auto *attr = m_reader->getAttribute(index);
    TU_ASSERT (attr != nullptr);
    if (attr->attr_value_type() != tts1::Value::StringValue)
        return tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kWrongType,"attr type mismatch");
    auto *value = attr->attr_value_as_StringValue();
    str = value->utf8()? value->utf8()->str() : std::string();
    return {};
}