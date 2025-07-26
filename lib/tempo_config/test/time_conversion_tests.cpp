#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_config/time_conversions.h>

TEST(TimeConversions, TestConvertTime)
{
    tempo_config::TimeParser parser(absl::RFC3339_full);
    absl::Time t;
    auto status = parser.convertValue(tempo_config::ConfigValue("2025-04-01T12:34:56Z"), t);
    ASSERT_TRUE (status.isOk());
    absl::CivilSecond ct(2025, 4, 1, 12, 34, 56);
    ASSERT_EQ (absl::FromCivil(ct, absl::UTCTimeZone()), t);
}

TEST(TimeConversions, TestConvertDuration)
{
    tempo_config::DurationParser parser;
    absl::Duration d;
    auto status = parser.convertValue(tempo_config::ConfigValue("12h"), d);
    ASSERT_TRUE (status.isOk());
    ASSERT_EQ (absl::Hours(12), d);
}

TEST(TimeConversions, TestConvertTimeZone)
{
    tempo_config::TimeZoneParser parser;
    absl::TimeZone tz;
    auto status = parser.convertValue(tempo_config::ConfigValue("America/Los_Angeles"), tz);
    ASSERT_TRUE (status.isOk());
    absl::TimeZone expected;
    ASSERT_TRUE (absl::LoadTimeZone("America/Los_Angeles", &expected));
    ASSERT_EQ (expected, tz);
}