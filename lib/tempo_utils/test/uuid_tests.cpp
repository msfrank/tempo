
#include <gtest/gtest.h>

#include <tempo_utils/uuid.h>

TEST(UUID, DefaultConstructedUUIDIsInvalid)
{
    tempo_utils::UUID uuid;
    ASSERT_FALSE (uuid.isValid());
}

TEST(UUID, NilUUIDIsValid)
{
    auto uuid = tempo_utils::UUID::nilUUID();
    ASSERT_TRUE (uuid.isValid());
    ASSERT_TRUE (uuid.isNil());
}

TEST(UUID, RandomUUIDIsValid)
{
    auto uuid = tempo_utils::UUID::randomUUID();
    ASSERT_TRUE (uuid.isValid());
}

TEST(UUID, ParseUUID)
{
    auto src = tempo_utils::UUID::randomUUID();
    auto str = src.toString();
    auto dst = tempo_utils::UUID::parse(str);

    ASSERT_TRUE (dst.isValid());
    ASSERT_EQ (src, dst);
    ASSERT_EQ (str, dst.toString());
}

TEST(UUID, ParseNilUUID)
{
    auto uuid = tempo_utils::UUID::parse("{00000000-0000-0000-0000-000000000000}");

    ASSERT_TRUE (uuid.isValid());
    ASSERT_TRUE (uuid.isNil());
}

TEST(UUID, ParseEmptyUUIDFails)
{
    auto uuid = tempo_utils::UUID::parse("");

    ASSERT_FALSE (uuid.isValid());
}

TEST(UUID, ParseInvalidUUIDFails)
{
    auto uuid = tempo_utils::UUID::parse("foobar");

    ASSERT_FALSE (uuid.isValid());
}