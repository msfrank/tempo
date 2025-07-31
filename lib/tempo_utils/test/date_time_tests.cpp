#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_utils/date_time.h>

class Duration : public ::testing::Test {};

TEST_F(Duration, MakeSecondsDuration)
{
    auto d = tempo_utils::Duration::seconds(3);
    ASSERT_EQ (3000000000, d.getNanos());
    ASSERT_EQ (3000000, d.getMicros());
    ASSERT_EQ (3000, d.getMillis());
    ASSERT_EQ (3, d.getSeconds());
    ASSERT_EQ (0, d.getMinutes());
    ASSERT_EQ (0, d.getHours());
}

TEST_F(Duration, MakeMillisDuration)
{
    auto d = tempo_utils::Duration::millis(3);
    ASSERT_EQ (3000000, d.getNanos());
    ASSERT_EQ (3000, d.getMicros());
    ASSERT_EQ (3, d.getMillis());
    ASSERT_EQ (0, d.getSeconds());
    ASSERT_EQ (0, d.getMinutes());
    ASSERT_EQ (0, d.getHours());
}

TEST_F(Duration, MakeMicrosDuration)
{
    auto d = tempo_utils::Duration::micros(3);
    ASSERT_EQ (3000, d.getNanos());
    ASSERT_EQ (3, d.getMicros());
    ASSERT_EQ (0, d.getMillis());
    ASSERT_EQ (0, d.getSeconds());
    ASSERT_EQ (0, d.getMinutes());
    ASSERT_EQ (0, d.getHours());
}

TEST_F(Duration, MakeNanosDuration)
{
    auto d = tempo_utils::Duration::nanos(3);
    ASSERT_EQ (3, d.getNanos());
    ASSERT_EQ (0, d.getMicros());
    ASSERT_EQ (0, d.getMillis());
    ASSERT_EQ (0, d.getSeconds());
    ASSERT_EQ (0, d.getMinutes());
    ASSERT_EQ (0, d.getHours());
}

TEST_F(Duration, AddSameScale)
{
    auto d1 = tempo_utils::Duration::seconds(3);
    auto d2 = tempo_utils::Duration::seconds(4);
    auto sum = d1 + d2;
    ASSERT_EQ (7, sum.getSeconds());
}

TEST_F(Duration, SubtractSameScale)
{
    auto d1 = tempo_utils::Duration::seconds(5);
    auto d2 = tempo_utils::Duration::seconds(4);
    auto sum = d1 - d2;
    ASSERT_EQ (1, sum.getSeconds());
}

TEST_F(Duration, AddDifferentScale)
{
    auto d1 = tempo_utils::Duration::seconds(3);
    auto d2 = tempo_utils::Duration::millis(4);
    auto sum = d1 + d2;
    ASSERT_EQ (3004, sum.getMillis());
}

TEST_F(Duration, SubtractDifferentScale)
{
    auto d1 = tempo_utils::Duration::seconds(5);
    auto d2 = tempo_utils::Duration::millis(4);
    auto sum = d1 - d2;
    ASSERT_EQ (4996, sum.getMillis());
}

class Instant : public ::testing::Test {};

TEST_F(Instant, MakeInstantFromEpochSeconds)
{
    auto t = tempo_utils::Instant::fromEpochSeconds(3);
    ASSERT_EQ (3, t.getEpochSeconds());
    ASSERT_EQ (3000, t.getEpochMillis());
}

TEST_F(Instant, MakeInstantFromEpochMillis)
{
    auto t = tempo_utils::Instant::fromEpochMillis(3);
    ASSERT_EQ (0, t.getEpochSeconds());
    ASSERT_EQ (3, t.getEpochMillis());
}
