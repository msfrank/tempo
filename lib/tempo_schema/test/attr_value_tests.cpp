#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_schema/attr.h>

TEST(AttrValue, ConstructInvalid)
{
    tempo_schema::AttrValue value;
    ASSERT_FALSE (value.isValid());
    ASSERT_EQ (tempo_schema::ValueType::Invalid, value.getType());
}

TEST(AttrValue, ConstructNil)
{
    tempo_schema::AttrValue value(nullptr);
    ASSERT_EQ (tempo_schema::ValueType::Nil, value.getType());
}

TEST(AttrValue, ConstructTrue)
{
    tempo_schema::AttrValue value(true);
    ASSERT_EQ (tempo_schema::ValueType::Bool, value.getType());
    ASSERT_EQ (true, value.getBool());
}

TEST(AttrValue, ConstructFalse)
{
    tempo_schema::AttrValue value(false);
    ASSERT_EQ (tempo_schema::ValueType::Bool, value.getType());
    ASSERT_EQ (false, value.getBool());
}

TEST(AttrValue, ConstructInt64)
{
    tempo_schema::AttrValue value(static_cast<tu_int64>(42));
    ASSERT_EQ (tempo_schema::ValueType::Int64, value.getType());
    ASSERT_EQ (42, value.getInt64());
}

TEST(AttrValue, ConstructFloat64)
{
    tempo_schema::AttrValue value(static_cast<double>(1.23));
    ASSERT_EQ (tempo_schema::ValueType::Float64, value.getType());
    ASSERT_EQ (1.23, value.getFloat64());
}

TEST(AttrValue, ConstructUInt64)
{
    tempo_schema::AttrValue value(static_cast<tu_uint64>(42));
    ASSERT_EQ (tempo_schema::ValueType::UInt64, value.getType());
    ASSERT_EQ (42, value.getUInt64());
}

TEST(AttrValue, ConstructUInt32)
{
    tempo_schema::AttrValue value(static_cast<tu_uint32>(42));
    ASSERT_EQ (tempo_schema::ValueType::UInt32, value.getType());
    ASSERT_EQ (42, value.getUInt32());
}

TEST(AttrValue, ConstructUInt16)
{
    tempo_schema::AttrValue value(static_cast<tu_uint16>(42));
    ASSERT_EQ (tempo_schema::ValueType::UInt16, value.getType());
    ASSERT_EQ (42, value.getUInt16());
}

TEST(AttrValue, ConstructUInt8)
{
    tempo_schema::AttrValue value(static_cast<tu_uint8>(42));
    ASSERT_EQ (tempo_schema::ValueType::UInt8, value.getType());
    ASSERT_EQ (42, value.getUInt8());
}

TEST(AttrValue, ConstructString)
{
    tempo_schema::AttrValue strValue(std::string{"hello, world!"});
    ASSERT_EQ (tempo_schema::ValueType::String, strValue.getType());
    ASSERT_EQ ("hello, world!", strValue.getString());

    tempo_schema::AttrValue svValue(std::string_view("hello, world!"));
    ASSERT_EQ (tempo_schema::ValueType::String, svValue.getType());
    ASSERT_EQ ("hello, world!", svValue.getString());

    tempo_schema::AttrValue rawValue("hello, world!");
    ASSERT_EQ (tempo_schema::ValueType::String, rawValue.getType());
    ASSERT_EQ ("hello, world!", rawValue.getString());
}

TEST(AttrValue, ConstructHandle)
{
    tempo_schema::AttrValue value(tempo_schema::AttrHandle(1));
    ASSERT_EQ (tempo_schema::ValueType::Handle, value.getType());
    ASSERT_EQ (tempo_schema::AttrHandle(1).handle, value.getHandle().handle);
}
