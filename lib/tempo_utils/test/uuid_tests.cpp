
#include <span>

#include <gtest/gtest.h>

#include <tempo_utils/uuid.h>

class UUID : public ::testing::Test {};

TEST_F(UUID, DefaultConstructedUUIDIsInvalid)
{
    tempo_utils::UUID uuid;
    ASSERT_FALSE (uuid.isValid());
}

TEST_F(UUID, NilUUIDIsValid)
{
    auto uuid = tempo_utils::UUID::nilUUID();
    ASSERT_TRUE (uuid.isValid());
    ASSERT_TRUE (uuid.isNil());
}

TEST_F(UUID, RandomUUIDIsValid)
{
    auto uuid = tempo_utils::UUID::randomUUID();
    ASSERT_TRUE (uuid.isValid());
}

TEST_F(UUID, RoundTrip)
{
    auto src = tempo_utils::UUID::randomUUID();
    auto str = src.toRfc4122String();
    auto dst = tempo_utils::UUID::parse(str);

    ASSERT_TRUE (dst.isValid());
    ASSERT_EQ (src, dst);
    ASSERT_EQ (str, dst.toRfc4122String());
    ASSERT_EQ (src.toBytes(), dst.toBytes());
}

TEST_F(UUID, NilUUIDIsLessThanNonNil)
{
    auto nil = tempo_utils::UUID::nilUUID();
    auto rand = tempo_utils::UUID::randomUUID();
    ASSERT_EQ (-1, nil.compare(rand));
    ASSERT_TRUE (nil < rand);
    ASSERT_TRUE (nil != rand);
    ASSERT_FALSE (nil == rand);
}

TEST_F(UUID, NilUUIDComparesEqualToSelf)
{
    auto nil = tempo_utils::UUID::nilUUID();
    ASSERT_EQ (0, nil.compare(nil));
    ASSERT_TRUE (nil == nil);
    ASSERT_FALSE (nil < nil);
    ASSERT_FALSE (nil != nil);
}

TEST_F(UUID, UUIDComparesEqualToSelf)
{
    auto rand = tempo_utils::UUID::randomUUID();
    ASSERT_EQ (0, rand.compare(rand));
    ASSERT_TRUE (rand == rand);
    ASSERT_FALSE (rand < rand);
    ASSERT_FALSE (rand != rand);
}

TEST_F(UUID, RandomUUIDsCompareUnequal)
{
    auto rand1 = tempo_utils::UUID::randomUUID();
    auto rand2 = tempo_utils::UUID::randomUUID();
    ASSERT_NE (0, rand1.compare(rand2));
    ASSERT_TRUE (rand1 != rand2);
    ASSERT_FALSE (rand1 == rand2);
}

TEST_F(UUID, ParseUUID)
{
    std::vector<tu_uint8> expected = { 1, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 5, 5, 5, 5};

    auto rfc4122 = tempo_utils::UUID::parse("01010101-0202-0303-0404-050505050505");
    ASSERT_EQ (expected, rfc4122.toBytes());

    auto compact = tempo_utils::UUID::parse("01010101020203030404050505050505");
    ASSERT_EQ (expected, rfc4122.toBytes());

    auto enclosed = tempo_utils::UUID::parse("{01010101-0202-0303-0404-050505050505}");
    ASSERT_EQ (expected, rfc4122.toBytes());

    auto enclosedCompact = tempo_utils::UUID::parse("{01010101020203030404-050505050505}");
    ASSERT_EQ (expected, rfc4122.toBytes());
}

TEST_F(UUID, ParseNilUUID)
{
    auto uuid = tempo_utils::UUID::parse("{00000000-0000-0000-0000-000000000000}");

    ASSERT_TRUE (uuid.isValid());
    ASSERT_TRUE (uuid.isNil());
}

TEST_F(UUID, ToRfc4122String)
{
    std::vector<tu_uint8> expected = { 1, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 5, 5, 5, 5};

    auto uuid = tempo_utils::UUID::parse("01010101-0202-0303-0404-050505050505");
    ASSERT_TRUE (uuid.isValid());
    ASSERT_EQ ("01010101-0202-0303-0404-050505050505", uuid.toRfc4122String());
}

TEST_F(UUID, ToCompactString)
{
    std::vector<tu_uint8> expected = { 1, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 5, 5, 5, 5};

    auto uuid = tempo_utils::UUID::parse("01010101020203030404050505050505");
    ASSERT_TRUE (uuid.isValid());
    ASSERT_EQ ("01010101020203030404050505050505", uuid.toCompactString());
}

TEST_F(UUID, ToEnclosedString)
{
    std::vector<tu_uint8> expected = { 1, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 5, 5, 5, 5};

    auto uuid = tempo_utils::UUID::parse("{01010101-0202-0303-0404-050505050505}");
    ASSERT_TRUE (uuid.isValid());
    ASSERT_EQ ("{01010101-0202-0303-0404-050505050505}", uuid.toEnclosedString());
}

TEST_F(UUID, ToEnclosedCompactString)
{
    std::vector<tu_uint8> expected = { 1, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 5, 5, 5, 5};

    auto uuid = tempo_utils::UUID::parse("{01010101020203030404050505050505}");
    ASSERT_TRUE (uuid.isValid());
    ASSERT_EQ ("{01010101020203030404050505050505}", uuid.toEnclosedCompactString());
}

TEST_F(UUID, ParseEmptyUUIDFails)
{
    auto uuid = tempo_utils::UUID::parse("");

    ASSERT_FALSE (uuid.isValid());
}

TEST_F(UUID, ParseInvalidUUIDFails)
{
    auto uuid = tempo_utils::UUID::parse("foobar");

    ASSERT_FALSE (uuid.isValid());
}