
#include <tempo_utils/attr.h>

bool tempo_utils::operator==(const AttrKey &lhs, const AttrKey &rhs)
{
    return lhs.id == rhs.id && std::string_view(lhs.ns) == std::string_view(rhs.ns);
}

tempo_utils::AttrValue::Priv::Priv()
    : type(ValueType::Invalid),
      value()
{
}

tempo_utils::AttrValue::Priv::~Priv()
{
    if (type == ValueType::String && value.str != nullptr) {
        auto *ptr = value.str - sizeof(std::string_view::size_type);
        delete ptr;
    }
}

tempo_utils::AttrValue::AttrValue()
    : m_priv()
{
}

tempo_utils::AttrValue::AttrValue(std::nullptr_t nil)
    : m_priv(std::make_shared<Priv>())
{
    m_priv->type = ValueType::Nil;
}

tempo_utils::AttrValue::AttrValue(bool b)
    : m_priv(std::make_shared<Priv>())
{
    m_priv->type = ValueType::Bool;
    m_priv->value.b = b;
}

tempo_utils::AttrValue::AttrValue(tu_int64 i64)
    : m_priv(std::make_shared<Priv>())
{
    m_priv->type = ValueType::Int64;
    m_priv->value.i64 = i64;
}

tempo_utils::AttrValue::AttrValue(double f64)
    : m_priv(std::make_shared<Priv>())
{
    m_priv->type = ValueType::Float64;
    m_priv->value.f64 = f64;
}

tempo_utils::AttrValue::AttrValue(tu_uint64 u64)
    : m_priv(std::make_shared<Priv>())
{
    m_priv->type = ValueType::UInt64;
    m_priv->value.u64 = u64;
}

tempo_utils::AttrValue::AttrValue(tu_uint32 u32)
    : m_priv(std::make_shared<Priv>())
{
    m_priv->type = ValueType::UInt32;
    m_priv->value.u32 = u32;
}

tempo_utils::AttrValue::AttrValue(tu_uint16 u16)
    : m_priv(std::make_shared<Priv>())
{
    m_priv->type = ValueType::UInt16;
    m_priv->value.u16 = u16;
}

tempo_utils::AttrValue::AttrValue(tu_uint8 u8)
    : m_priv(std::make_shared<Priv>())
{
    m_priv->type = ValueType::UInt8;
    m_priv->value.u8 = u8;
}

tempo_utils::AttrValue::AttrValue(std::string_view str)
    : m_priv(std::make_shared<Priv>())
{
    m_priv->type = ValueType::String;
    if (!str.empty()) {
        auto size = str.size() + sizeof(std::string_view::size_type) + 1;
        auto *bytes = new char[size];
        auto *len = (std::string_view::size_type *) bytes;
        *len = str.size();
        m_priv->value.str = bytes + sizeof(std::string_view::size_type);
        std::memcpy((void *) m_priv->value.str, str.data(), str.size());
        bytes[size - 1] = '\0';
    } else {
        m_priv->value.str = nullptr;
    }
}

tempo_utils::AttrValue::AttrValue(const std::string &str)
    : AttrValue(std::string_view(str))
{
}

tempo_utils::AttrValue::AttrValue(const char *str)
    : AttrValue(std::string_view(str))
{
}

tempo_utils::AttrValue::AttrValue(const AttrValue &other)
    : m_priv(other.m_priv)
{
}

tempo_utils::AttrValue::AttrValue(AttrValue &&other) noexcept
{
    m_priv.swap(other.m_priv);
}

tempo_utils::AttrValue&
tempo_utils::AttrValue::operator=(const AttrValue &other)
{
    if (this != &other) {
        m_priv = other.m_priv;
    }
    return *this;
}

tempo_utils::AttrValue&
tempo_utils::AttrValue::operator=(AttrValue &&other) noexcept
{
    if (this != &other) {
        m_priv.swap(other.m_priv);
    }
    return *this;
}

bool
tempo_utils::AttrValue::isValid() const
{
    return m_priv != nullptr;
}

tempo_utils::ValueType
tempo_utils::AttrValue::getType() const
{
    if (!isValid())
        return ValueType::Invalid;
    return m_priv->type;
}

bool
tempo_utils::AttrValue::getBool() const
{
    return getType() == ValueType::Bool && m_priv->value.b;
}

tu_int64
tempo_utils::AttrValue::getInt64() const
{
    return getType() == ValueType::Int64? m_priv->value.i64 : 0;
}

double
tempo_utils::AttrValue::getFloat64() const
{
    return getType() == ValueType::Float64? m_priv->value.f64 : 0;
}

tu_uint64
tempo_utils::AttrValue::getUInt64() const
{
    return getType() == ValueType::UInt64? m_priv->value.u64 : 0;
}

tu_uint32
tempo_utils::AttrValue::getUInt32() const
{
    return getType() == ValueType::UInt32? m_priv->value.u32 : 0;
}

tu_uint16
tempo_utils::AttrValue::getUInt16() const
{
    return getType() == ValueType::UInt16? m_priv->value.u16 : 0;
}

tu_uint8
tempo_utils::AttrValue::getUInt8() const
{
    return getType() == ValueType::UInt8? m_priv->value.u8 : 0;
}

std::string
tempo_utils::AttrValue::getString() const
{
    if (getType() == ValueType::String && m_priv->value.str != nullptr) {
        auto *ptr = m_priv->value.str - sizeof(std::string_view::size_type);
        auto *len = (const std::string_view::size_type *) ptr;
        return std::string(m_priv->value.str, *len);
    }
    return {};
}

std::string_view
tempo_utils::AttrValue::stringView() const
{
    if (getType() == ValueType::String && m_priv->value.str != nullptr) {
        auto *ptr = m_priv->value.str - sizeof(std::string_view::size_type);
        auto *len = (const std::string_view::size_type *) ptr;
        return std::string_view (m_priv->value.str, *len);
    }
    return {};
}

tempo_utils::AttrValidator::AttrValidator(const ComparableResource *resource)
    : m_resource(resource)
{
    TU_ASSERT (m_resource != nullptr);
}

tempo_utils::Result<tu_uint32>
tempo_utils::AbstractAttrWriter::putInt32(tu_int32 i32)
{
    return putInt64(static_cast<tu_int64>(i32));
}

tempo_utils::Result<tu_uint32>
tempo_utils::AbstractAttrWriter::putInt16(tu_int16 i16)
{
    return putInt64(static_cast<tu_int64>(i16));
}

tempo_utils::Result<tu_uint32>
tempo_utils::AbstractAttrWriter::putInt8(tu_int8 i8)
{
    return putInt64(static_cast<tu_int64>(i8));
}

tempo_utils::Result<tu_uint32>
tempo_utils::AbstractAttrWriter::putUInt32(tu_uint32 u32)
{
    return putUInt64(static_cast<tu_uint64>(u32));
}

tempo_utils::Result<tu_uint32>
tempo_utils::AbstractAttrWriter::putUInt16(tu_uint16 u16)
{
    return putUInt64(static_cast<tu_uint64>(u16));
}

tempo_utils::Result<tu_uint32>
tempo_utils::AbstractAttrWriter::putUInt8(tu_uint8 u8)
{
    return putUInt64(static_cast<tu_uint64>(u8));
}

tempo_utils::Status
tempo_utils::AbstractAttrParser::getInt32(tu_uint32 index, tu_int32 &i32)
{
    tu_int64 value;
    auto status = getInt64(index, value);
    if (status.notOk())
        return status;
    if (value < std::numeric_limits<tu_int32>::min() || value > std::numeric_limits<tu_int32>::max())
        return AttrStatus::forCondition(AttrCondition::kConversionError, "out of range");
    i32 = static_cast<tu_int32>(value);
    return AttrStatus::ok();
}

tempo_utils::Status
tempo_utils::AbstractAttrParser::getInt16(tu_uint32 index, tu_int16 &i16)
{
    tu_int64 value;
    auto status = getInt64(index, value);
    if (status.notOk())
        return status;
    if (value < std::numeric_limits<tu_int16>::min() || value > std::numeric_limits<tu_int16>::max())
        return AttrStatus::forCondition(AttrCondition::kConversionError, "out of range");
    i16 = static_cast<tu_int16>(value);
    return AttrStatus::ok();
}

tempo_utils::Status
tempo_utils::AbstractAttrParser::getInt8(tu_uint32 index, tu_int8 &i8)
{
    tu_int64 value;
    auto status = getInt64(index, value);
    if (status.notOk())
        return status;
    if (value < std::numeric_limits<tu_int8>::min() || value > std::numeric_limits<tu_int8>::max())
        return AttrStatus::forCondition(AttrCondition::kConversionError, "out of range");
    i8 = static_cast<tu_int8>(value);
    return AttrStatus::ok();
}

tempo_utils::Status
tempo_utils::AbstractAttrParser::getUInt32(tu_uint32 index, tu_uint32 &u32)
{
    tu_int64 value;
    auto status = getInt64(index, value);
    if (status.notOk())
        return status;
    if (value < std::numeric_limits<tu_uint32>::min() || value > std::numeric_limits<tu_uint32>::max())
        return AttrStatus::forCondition(AttrCondition::kConversionError, "out of range");
    u32 = static_cast<tu_uint32>(value);
    return AttrStatus::ok();
}

tempo_utils::Status
tempo_utils::AbstractAttrParser::getUInt16(tu_uint32 index, tu_uint16 &u16)
{
    tu_int64 value;
    auto status = getInt64(index, value);
    if (status.notOk())
        return status;
    if (value < std::numeric_limits<tu_uint16>::min() || value > std::numeric_limits<tu_uint16>::max())
        return AttrStatus::forCondition(AttrCondition::kConversionError, "out of range");
    u16 = static_cast<tu_uint16>(value);
    return AttrStatus::ok();
}

tempo_utils::Status
tempo_utils::AbstractAttrParser::getUInt8(tu_uint32 index, tu_uint8 &u8)
{
    tu_int64 value;
    auto status = getInt64(index, value);
    if (status.notOk())
        return status;
    if (value < std::numeric_limits<tu_uint8>::min() || value > std::numeric_limits<tu_uint8>::max())
        return AttrStatus::forCondition(AttrCondition::kConversionError, "out of range");
    u8 = static_cast<tu_uint8>(value);
    return AttrStatus::ok();
}

const tempo_utils::ComparableResource *
tempo_utils::AttrValidator::getResource() const
{
    return m_resource;
}

tempo_utils::AttrKey
tempo_utils::AttrValidator::getKey() const
{
    return AttrKey{m_resource->getNsUrl(), m_resource->getIdValue()};
}

tempo_utils::NilAttr::NilAttr(const ComparableResource *resource)
    : AttrSerde<std::nullptr_t>(resource)
{
}

//tempo_utils::Result<tempo_utils::AttrValue>
//tempo_utils::NilAttr::writeAttr(const std::nullptr_t &value) const
//{
//    return Result<AttrValue>(AttrValue(value));
//}

tempo_utils::Result<tu_uint32>
tempo_utils::NilAttr::writeAttr(AbstractAttrWriter *writer, const std::nullptr_t &value) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putNil();
}

//tempo_utils::Result<std::nullptr_t>
//tempo_utils::NilAttr::parseAttr(const tempo_utils::AttrValue &attr) const
//{
//    const auto *nil = boost::get<std::nullptr_t>(&attr);
//    if (nil == nullptr)
//        return Status::wrongType("expected null type");
//    return Result<std::nullptr_t>(*nil);
//}

tempo_utils::Status
tempo_utils::NilAttr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, std::nullptr_t &value) const
{
    return parser->getNil(index, value);
}

tempo_utils::Status
tempo_utils::NilAttr::validateAttr(tu_uint32 index, AbstractAttrParser *parser) const
{
    std::nullptr_t value;
    return parser->getNil(index, value);
}

std::string
tempo_utils::NilAttr::toString(tu_uint32 index, AbstractAttrParser *parser) const
{
    std::nullptr_t value;
    auto status = parser->getNil(index, value);
    if (status.isOk())
        return "nil";
    return "???";
}

tempo_utils::BoolAttr::BoolAttr(const ComparableResource *resource)
    : AttrSerde<bool>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_utils::BoolAttr::writeAttr(AbstractAttrWriter *writer, const bool &value) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putBool(value);
}

tempo_utils::Status
tempo_utils::BoolAttr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, bool &value) const
{
    return parser->getBool(index, value);
}

tempo_utils::Status
tempo_utils::BoolAttr::validateAttr(tu_uint32 index, AbstractAttrParser *parser) const
{
    bool value;
    return parser->getBool(index, value);
}

std::string
tempo_utils::BoolAttr::toString(tu_uint32 index, AbstractAttrParser *parser) const
{
    bool value;
    auto status = parser->getBool(index, value);
    if (status.isOk())
        return value? "true" : "false";
    return "???";
}

tempo_utils::Int64Attr::Int64Attr(const ComparableResource *resource)
    : AttrSerde<int64_t>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_utils::Int64Attr::writeAttr(AbstractAttrWriter *writer, const int64_t &value) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putInt64(value);
}

tempo_utils::Status
tempo_utils::Int64Attr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, int64_t &value) const
{
    return parser->getInt64(index, value);
}

tempo_utils::Status
tempo_utils::Int64Attr::validateAttr(tu_uint32 index, AbstractAttrParser *parser) const
{
    tu_int64 value;
    return parser->getInt64(index, value);
}

std::string
tempo_utils::Int64Attr::toString(tu_uint32 index, AbstractAttrParser *parser) const
{
    tu_int64 value;
    auto status = parser->getInt64(index, value);
    if (status.isOk())
        return absl::StrCat(value);
    return "???";
}

tempo_utils::Int32Attr::Int32Attr(const ComparableResource *resource)
    : AttrSerde<tu_int32>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_utils::Int32Attr::writeAttr(AbstractAttrWriter *writer, const tu_int32 &value) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putInt32(value);
}

tempo_utils::Status
tempo_utils::Int32Attr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, tu_int32 &value) const
{
    return parser->getInt32(index, value);
}

tempo_utils::Status
tempo_utils::Int32Attr::validateAttr(tu_uint32 index, AbstractAttrParser *parser) const
{
    tu_int32 value;
    return parser->getInt32(index, value);
}

std::string
tempo_utils::Int32Attr::toString(tu_uint32 index, AbstractAttrParser *parser) const
{
    tu_int32 value;
    auto status = parser->getInt32(index, value);
    if (status.isOk())
        return absl::StrCat(value);
    return "???";
}

tempo_utils::Int16Attr::Int16Attr(const ComparableResource *resource)
    : AttrSerde<tu_int16>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_utils::Int16Attr::writeAttr(AbstractAttrWriter *writer, const tu_int16 &value) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putInt16(value);
}

tempo_utils::Status
tempo_utils::Int16Attr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, tu_int16 &value) const
{
    return parser->getInt16(index, value);
}

tempo_utils::Status
tempo_utils::Int16Attr::validateAttr(tu_uint32 index, AbstractAttrParser *parser) const
{
    tu_int16 value;
    return parser->getInt16(index, value);
}

std::string
tempo_utils::Int16Attr::toString(tu_uint32 index, AbstractAttrParser *parser) const
{
    tu_int16 value;
    auto status = parser->getInt16(index, value);
    if (status.isOk())
        return absl::StrCat(value);
    return "???";
}

tempo_utils::Int8Attr::Int8Attr(const ComparableResource *resource)
    : AttrSerde<tu_int8>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_utils::Int8Attr::writeAttr(AbstractAttrWriter *writer, const tu_int8 &value) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putInt8(value);
}

tempo_utils::Status
tempo_utils::Int8Attr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, tu_int8 &value) const
{
    return parser->getInt8(index, value);
}

tempo_utils::Status
tempo_utils::Int8Attr::validateAttr(tu_uint32 index, AbstractAttrParser *parser) const
{
    tu_int8 value;
    return parser->getInt8(index, value);
}

std::string
tempo_utils::Int8Attr::toString(tu_uint32 index, AbstractAttrParser *parser) const
{
    tu_int8 value;
    auto status = parser->getInt8(index, value);
    if (status.isOk())
        return absl::StrCat(value);
    return "???";
}

tempo_utils::UInt64Attr::UInt64Attr(const ComparableResource *resource)
    : AttrSerde<uint64_t>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_utils::UInt64Attr::writeAttr(AbstractAttrWriter *writer, const uint64_t &value) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putUInt64(value);
}

tempo_utils::Status
tempo_utils::UInt64Attr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, uint64_t &value) const
{
    return parser->getUInt64(index, value);
}

tempo_utils::Status
tempo_utils::UInt64Attr::validateAttr(tu_uint32 index, AbstractAttrParser *parser) const
{
    tu_uint64 value;
    return parser->getUInt64(index, value);
}

std::string
tempo_utils::UInt64Attr::toString(tu_uint32 index, AbstractAttrParser *parser) const
{
    tu_uint64 value;
    auto status = parser->getUInt64(index, value);
    if (status.isOk())
        return absl::StrCat(value);
    return "???";
}

tempo_utils::UInt32Attr::UInt32Attr(const ComparableResource *resource)
    : AttrSerde<tu_uint32>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_utils::UInt32Attr::writeAttr(AbstractAttrWriter *writer, const tu_uint32 &value) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putUInt32(value);
}

tempo_utils::Status
tempo_utils::UInt32Attr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, tu_uint32 &value) const
{
    return parser->getUInt32(index, value);
}

tempo_utils::Status
tempo_utils::UInt32Attr::validateAttr(tu_uint32 index, AbstractAttrParser *parser) const
{
    tu_uint32 value;
    return parser->getUInt32(index, value);
}

std::string
tempo_utils::UInt32Attr::toString(tu_uint32 index, AbstractAttrParser *parser) const
{
    tu_uint32 value;
    auto status = parser->getUInt32(index, value);
    if (status.isOk())
        return absl::StrCat(value);
    return "???";
}

tempo_utils::UInt16Attr::UInt16Attr(const ComparableResource *resource)
    : AttrSerde<tu_uint16>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_utils::UInt16Attr::writeAttr(AbstractAttrWriter *writer, const tu_uint16 &value) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putUInt16(value);
}

tempo_utils::Status
tempo_utils::UInt16Attr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, tu_uint16 &value) const
{
    return parser->getUInt16(index, value);
}

tempo_utils::Status
tempo_utils::UInt16Attr::validateAttr(tu_uint32 index, AbstractAttrParser *parser) const
{
    tu_uint16 value;
    return parser->getUInt16(index, value);
}

std::string
tempo_utils::UInt16Attr::toString(tu_uint32 index, AbstractAttrParser *parser) const
{
    tu_uint16 value;
    auto status = parser->getUInt16(index, value);
    if (status.isOk())
        return absl::StrCat(value);
    return "???";
}

tempo_utils::UInt8Attr::UInt8Attr(const ComparableResource *resource)
    : AttrSerde<tu_uint8>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_utils::UInt8Attr::writeAttr(AbstractAttrWriter *writer, const tu_uint8 &value) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putUInt8(value);
}

tempo_utils::Status
tempo_utils::UInt8Attr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, tu_uint8 &value) const
{
    return parser->getUInt8(index, value);
}

tempo_utils::Status
tempo_utils::UInt8Attr::validateAttr(tu_uint32 index, AbstractAttrParser *parser) const
{
    tu_uint8 value;
    return parser->getUInt8(index, value);
}

std::string
tempo_utils::UInt8Attr::toString(tu_uint32 index, AbstractAttrParser *parser) const
{
    tu_uint8 value;
    auto status = parser->getUInt8(index, value);
    if (status.isOk())
        return absl::StrCat(value);
    return "???";
}

tempo_utils::DoubleAttr::DoubleAttr(const ComparableResource *resource)
    : AttrSerde<double>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_utils::DoubleAttr::writeAttr(AbstractAttrWriter *writer, const double &value) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putFloat64(value);
}

tempo_utils::Status
tempo_utils::DoubleAttr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, double &value) const
{
    return parser->getFloat64(index, value);
}

tempo_utils::Status
tempo_utils::DoubleAttr::validateAttr(tu_uint32 index, AbstractAttrParser *parser) const
{
    double value;
    return parser->getFloat64(index, value);
}

std::string
tempo_utils::DoubleAttr::toString(tu_uint32 index, AbstractAttrParser *parser) const
{
    double value;
    auto status = parser->getFloat64(index, value);
    if (status.isOk())
        return absl::StrCat(value);
    return "???";
}

tempo_utils::StringAttr::StringAttr(const ComparableResource *resource)
    : AttrSerde<std::string>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_utils::StringAttr::writeAttr(AbstractAttrWriter *writer, const std::string &value) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putString(value);
}

tempo_utils::Status
tempo_utils::StringAttr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, std::string &value) const
{
    return parser->getString(index, value);
}

tempo_utils::Status
tempo_utils::StringAttr::validateAttr(tu_uint32 index, AbstractAttrParser *parser) const
{
    std::string value;
    return parser->getString(index, value);
}

std::string
tempo_utils::StringAttr::toString(tu_uint32 index, AbstractAttrParser *parser) const
{
    std::string value;
    auto status = parser->getString(index, value);
    if (status.isOk())
        return value;
    return "???";
}