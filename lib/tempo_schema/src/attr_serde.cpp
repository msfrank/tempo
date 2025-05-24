
#include <tempo_schema/attr_serde.h>

tempo_schema::AttrValidator::AttrValidator(const ComparableResource *resource)
    : m_resource(resource)
{
    TU_ASSERT (m_resource != nullptr);
}

const tempo_schema::ComparableResource *
tempo_schema::AttrValidator::getResource() const
{
    return m_resource;
}

tempo_schema::AttrKey
tempo_schema::AttrValidator::getKey() const
{
    return AttrKey(m_resource->getNsUrl(), m_resource->getIdValue());
}

tempo_schema::NilAttr::NilAttr(const ComparableResource *resource)
    : AttrSerde<std::nullptr_t>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_schema::NilAttr::writeAttr(AbstractAttrWriter *writer, const std::nullptr_t &value) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putNil();
}

tempo_utils::Status
tempo_schema::NilAttr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, std::nullptr_t &value) const
{
    return parser->getNil(index, value);
}

tempo_schema::BoolAttr::BoolAttr(const ComparableResource *resource)
    : AttrSerde<bool>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_schema::BoolAttr::writeAttr(AbstractAttrWriter *writer, const bool &value) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putBool(value);
}

tempo_utils::Status
tempo_schema::BoolAttr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, bool &value) const
{
    return parser->getBool(index, value);
}

tempo_schema::Int64Attr::Int64Attr(const ComparableResource *resource)
    : AttrSerde<int64_t>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_schema::Int64Attr::writeAttr(AbstractAttrWriter *writer, const int64_t &value) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putInt64(value);
}

tempo_utils::Status
tempo_schema::Int64Attr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, int64_t &value) const
{
    return parser->getInt64(index, value);
}

tempo_schema::UInt64Attr::UInt64Attr(const ComparableResource *resource)
    : AttrSerde<uint64_t>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_schema::UInt64Attr::writeAttr(AbstractAttrWriter *writer, const uint64_t &value) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putUInt64(value);
}

tempo_utils::Status
tempo_schema::UInt64Attr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, uint64_t &value) const
{
    return parser->getUInt64(index, value);
}

tempo_schema::UInt32Attr::UInt32Attr(const ComparableResource *resource)
    : AttrSerde<tu_uint32>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_schema::UInt32Attr::writeAttr(AbstractAttrWriter *writer, const tu_uint32 &value) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putUInt32(value);
}

tempo_utils::Status
tempo_schema::UInt32Attr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, tu_uint32 &value) const
{
    return parser->getUInt32(index, value);
}

tempo_schema::UInt16Attr::UInt16Attr(const ComparableResource *resource)
    : AttrSerde<tu_uint16>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_schema::UInt16Attr::writeAttr(AbstractAttrWriter *writer, const tu_uint16 &value) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putUInt16(value);
}

tempo_utils::Status
tempo_schema::UInt16Attr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, tu_uint16 &value) const
{
    return parser->getUInt16(index, value);
}

tempo_schema::UInt8Attr::UInt8Attr(const ComparableResource *resource)
    : AttrSerde<tu_uint8>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_schema::UInt8Attr::writeAttr(AbstractAttrWriter *writer, const tu_uint8 &value) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putUInt8(value);
}

tempo_utils::Status
tempo_schema::UInt8Attr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, tu_uint8 &value) const
{
    return parser->getUInt8(index, value);
}

tempo_schema::DoubleAttr::DoubleAttr(const ComparableResource *resource)
    : AttrSerde<double>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_schema::DoubleAttr::writeAttr(AbstractAttrWriter *writer, const double &value) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putFloat64(value);
}

tempo_utils::Status
tempo_schema::DoubleAttr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, double &value) const
{
    return parser->getFloat64(index, value);
}

tempo_schema::StringAttr::StringAttr(const ComparableResource *resource)
    : AttrSerde<std::string>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_schema::StringAttr::writeAttr(AbstractAttrWriter *writer, const std::string &value) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putString(value);
}

tempo_utils::Status
tempo_schema::StringAttr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, std::string &value) const
{
    return parser->getString(index, value);
}
