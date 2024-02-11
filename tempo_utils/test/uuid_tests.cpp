
#include <gtest/gtest.h>

#include <tempo_utils/uuid.h>

TEST(UUID, DefaultConstructedUUIDIsInvalid)
{
    tempo_utils::UUID uuid;
    ASSERT_FALSE (uuid.isValid());
}

TEST(UUID, RandomUUIDIsValid)
{
    auto uuid = tempo_utils::UUID::randomUUID();
    ASSERT_TRUE (uuid.isValid());
}