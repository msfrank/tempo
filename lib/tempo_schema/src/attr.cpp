
#include <tempo_schema/attr.h>

bool tempo_schema::operator==(const AttrKey &lhs, const AttrKey &rhs)
{
    return lhs.id == rhs.id && std::string_view(lhs.ns) == std::string_view(rhs.ns);
}

tempo_schema::AttrValue::Priv::Priv(ValueType type, ValueVariant value)
    : type(type),
      value(std::move(value))
{
}

tempo_schema::AttrValue::AttrValue()
{
    m_priv = std::make_shared<const Priv>(ValueType::Invalid, ValueVariant{InvalidValue{}});
}

tempo_schema::AttrValue::AttrValue(std::nullptr_t)
{
    m_priv = std::make_shared<const Priv>(ValueType::Nil, ValueVariant{nullptr});
}

tempo_schema::AttrValue::AttrValue(bool b)
{
    m_priv = std::make_shared<const Priv>(ValueType::Bool, ValueVariant{b});
}

tempo_schema::AttrValue::AttrValue(tu_int64 i64)
{
    m_priv = std::make_shared<const Priv>(ValueType::Int64, ValueVariant{i64});
}

tempo_schema::AttrValue::AttrValue(double f64)
{
    m_priv = std::make_shared<const Priv>(ValueType::Float64, ValueVariant{f64});
}

tempo_schema::AttrValue::AttrValue(tu_uint64 u64)
{
    m_priv = std::make_shared<const Priv>(ValueType::UInt64, ValueVariant{u64});
}

tempo_schema::AttrValue::AttrValue(tu_uint32 u32)
{
    m_priv = std::make_shared<const Priv>(ValueType::UInt32, ValueVariant{u32});
}

tempo_schema::AttrValue::AttrValue(tu_uint16 u16)
{
    m_priv = std::make_shared<const Priv>(ValueType::UInt16, ValueVariant{u16});
}

tempo_schema::AttrValue::AttrValue(tu_uint8 u8)
{
    m_priv = std::make_shared<const Priv>(ValueType::UInt8, ValueVariant{u8});
}

tempo_schema::AttrValue::AttrValue(std::string_view str)
{
    m_priv = std::make_shared<const Priv>(ValueType::String, ValueVariant{std::string(str)});
}

tempo_schema::AttrValue::AttrValue(const std::string &str)
{
    m_priv = std::make_shared<const Priv>(ValueType::String, ValueVariant{str});
}

tempo_schema::AttrValue::AttrValue(const char *str)
{
    m_priv = std::make_shared<const Priv>(ValueType::String, ValueVariant{std::string(str)});
}

tempo_schema::AttrValue::AttrValue(AttrHandle handle)
{
    m_priv = std::make_shared<const Priv>(ValueType::Handle, ValueVariant{handle});
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
    return m_priv->type != ValueType::Invalid;
}

tempo_schema::ValueType
tempo_schema::AttrValue::getType() const
{
    return m_priv->type;
}

bool
tempo_schema::AttrValue::getBool() const
{
    return m_priv->type == ValueType::Bool && std::get<bool>(m_priv->value);
}

tu_int64
tempo_schema::AttrValue::getInt64() const
{
    return m_priv->type == ValueType::Int64? std::get<tu_int64>(m_priv->value) : 0;
}

double
tempo_schema::AttrValue::getFloat64() const
{
    return m_priv->type == ValueType::Float64? std::get<double>(m_priv->value) : 0;
}

tu_uint64
tempo_schema::AttrValue::getUInt64() const
{
    return m_priv->type == ValueType::UInt64? std::get<tu_uint64>(m_priv->value) : 0;
}

tu_uint32
tempo_schema::AttrValue::getUInt32() const
{
    return m_priv->type == ValueType::UInt32? std::get<tu_uint32>(m_priv->value) : 0;
}

tu_uint16
tempo_schema::AttrValue::getUInt16() const
{
    return m_priv->type == ValueType::UInt16? std::get<tu_uint16>(m_priv->value) : 0;
}

tu_uint8
tempo_schema::AttrValue::getUInt8() const
{
    return m_priv->type == ValueType::UInt8? std::get<tu_uint8>(m_priv->value) : 0;
}

std::string
tempo_schema::AttrValue::getString() const
{
    return m_priv->type == ValueType::String? std::get<std::string>(m_priv->value) : std::string{};
}

std::string_view
tempo_schema::AttrValue::stringView() const
{
    if (m_priv->type == ValueType::String) {
        auto &str = std::get<std::string>(m_priv->value);
        return std::string_view (str.data(), str.size());
    }
    return {};
}

tempo_schema::AttrHandle
tempo_schema::AttrValue::getHandle() const
{
    return getType() == ValueType::Handle? std::get<AttrHandle>(m_priv->value) : AttrHandle{0xFFFFFFFF};
}