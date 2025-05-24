
#include <tempo_schema/attr.h>

bool tempo_schema::operator==(const AttrKey &lhs, const AttrKey &rhs)
{
    return lhs.id == rhs.id && std::string_view(lhs.ns) == std::string_view(rhs.ns);
}

tempo_schema::AttrValue::Priv::Priv()
    : type(ValueType::Invalid),
      value()
{
}

tempo_schema::AttrValue::Priv::~Priv()
{
    if (type == ValueType::String && value.str != nullptr) {
        auto *ptr = value.str - sizeof(std::string_view::size_type);
        delete ptr;
    }
}

tempo_schema::AttrValue::AttrValue()
    : m_priv()
{
}

tempo_schema::AttrValue::AttrValue(std::nullptr_t nil)
    : m_priv(std::make_shared<Priv>())
{
    m_priv->type = ValueType::Nil;
}

tempo_schema::AttrValue::AttrValue(bool b)
    : m_priv(std::make_shared<Priv>())
{
    m_priv->type = ValueType::Bool;
    m_priv->value.b = b;
}

tempo_schema::AttrValue::AttrValue(tu_int64 i64)
    : m_priv(std::make_shared<Priv>())
{
    m_priv->type = ValueType::Int64;
    m_priv->value.i64 = i64;
}

tempo_schema::AttrValue::AttrValue(double f64)
    : m_priv(std::make_shared<Priv>())
{
    m_priv->type = ValueType::Float64;
    m_priv->value.f64 = f64;
}

tempo_schema::AttrValue::AttrValue(tu_uint64 u64)
    : m_priv(std::make_shared<Priv>())
{
    m_priv->type = ValueType::UInt64;
    m_priv->value.u64 = u64;
}

tempo_schema::AttrValue::AttrValue(tu_uint32 u32)
    : m_priv(std::make_shared<Priv>())
{
    m_priv->type = ValueType::UInt32;
    m_priv->value.u32 = u32;
}

tempo_schema::AttrValue::AttrValue(tu_uint16 u16)
    : m_priv(std::make_shared<Priv>())
{
    m_priv->type = ValueType::UInt16;
    m_priv->value.u16 = u16;
}

tempo_schema::AttrValue::AttrValue(tu_uint8 u8)
    : m_priv(std::make_shared<Priv>())
{
    m_priv->type = ValueType::UInt8;
    m_priv->value.u8 = u8;
}

tempo_schema::AttrValue::AttrValue(std::string_view str)
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

tempo_schema::AttrValue::AttrValue(const std::string &str)
    : AttrValue(std::string_view(str))
{
}

tempo_schema::AttrValue::AttrValue(const char *str)
    : AttrValue(std::string_view(str))
{
}

tempo_schema::AttrValue::AttrValue(AttrHandle handle)
    : m_priv(std::make_shared<Priv>())
{
    m_priv->type = ValueType::Handle;
    m_priv->value.handle = handle;
}

tempo_schema::AttrValue::AttrValue(const AttrValue &other)
    : m_priv(other.m_priv)
{
}

tempo_schema::AttrValue::AttrValue(AttrValue &&other) noexcept
{
    m_priv.swap(other.m_priv);
}

tempo_schema::AttrValue&
tempo_schema::AttrValue::operator=(const AttrValue &other)
{
    if (this != &other) {
        m_priv = other.m_priv;
    }
    return *this;
}

tempo_schema::AttrValue&
tempo_schema::AttrValue::operator=(AttrValue &&other) noexcept
{
    if (this != &other) {
        m_priv.swap(other.m_priv);
    }
    return *this;
}

bool
tempo_schema::AttrValue::isValid() const
{
    return m_priv != nullptr;
}

tempo_schema::ValueType
tempo_schema::AttrValue::getType() const
{
    if (!isValid())
        return ValueType::Invalid;
    return m_priv->type;
}

bool
tempo_schema::AttrValue::getBool() const
{
    return getType() == ValueType::Bool && m_priv->value.b;
}

tu_int64
tempo_schema::AttrValue::getInt64() const
{
    return getType() == ValueType::Int64? m_priv->value.i64 : 0;
}

double
tempo_schema::AttrValue::getFloat64() const
{
    return getType() == ValueType::Float64? m_priv->value.f64 : 0;
}

tu_uint64
tempo_schema::AttrValue::getUInt64() const
{
    return getType() == ValueType::UInt64? m_priv->value.u64 : 0;
}

tu_uint32
tempo_schema::AttrValue::getUInt32() const
{
    return getType() == ValueType::UInt32? m_priv->value.u32 : 0;
}

tu_uint16
tempo_schema::AttrValue::getUInt16() const
{
    return getType() == ValueType::UInt16? m_priv->value.u16 : 0;
}

tu_uint8
tempo_schema::AttrValue::getUInt8() const
{
    return getType() == ValueType::UInt8? m_priv->value.u8 : 0;
}

std::string
tempo_schema::AttrValue::getString() const
{
    if (getType() == ValueType::String && m_priv->value.str != nullptr) {
        auto *ptr = m_priv->value.str - sizeof(std::string_view::size_type);
        auto *len = (const std::string_view::size_type *) ptr;
        return std::string(m_priv->value.str, *len);
    }
    return {};
}

std::string_view
tempo_schema::AttrValue::stringView() const
{
    if (getType() == ValueType::String && m_priv->value.str != nullptr) {
        auto *ptr = m_priv->value.str - sizeof(std::string_view::size_type);
        auto *len = (const std::string_view::size_type *) ptr;
        return std::string_view (m_priv->value.str, *len);
    }
    return {};
}

tempo_schema::AttrHandle
tempo_schema::AttrValue::getHandle() const
{
    return getType() == ValueType::Handle? m_priv->value.handle : AttrHandle{0};
}