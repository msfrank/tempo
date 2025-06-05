
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_config/config_serde.h>
#include <tempo_config/config_result.h>
#include <tempo_test/result_matchers.h>

TEST(ConfigListenerErrorHandling, ErrorUnrecognizedSymbol)
{
    auto readConfigResult = tempo_config::read_config_string("#");
    ASSERT_THAT (readConfigResult, tempo_test::ContainsStatus(tempo_config::ConfigCondition::kParseError));
    TU_LOG_INFO << readConfigResult.getStatus();
}

TEST(ConfigListenerErrorHandling, ErrorMissingClosingBracket)
{
    auto readConfigResult = tempo_config::read_config_string(R"(
[ 1, 2, 3
    )");
    ASSERT_THAT (readConfigResult, tempo_test::ContainsStatus(tempo_config::ConfigCondition::kParseError));
    TU_LOG_INFO << readConfigResult.getStatus();
}

TEST(ConfigListenerErrorHandling, ErrorExtraClosingBracket)
{
    auto readConfigResult = tempo_config::read_config_string(R"(
[ 1, 2, 3 ] ]
    )");
    ASSERT_THAT (readConfigResult, tempo_test::ContainsStatus(tempo_config::ConfigCondition::kParseError));
    TU_LOG_INFO << readConfigResult.getStatus();
}

TEST(ConfigListenerErrorHandling, ErrorLeadingCommaBeforeFirstArrayElement)
{
    auto readConfigResult = tempo_config::read_config_string(R"(
[ , 1, 2, 3 ]
    )");
    ASSERT_THAT (readConfigResult, tempo_test::ContainsStatus(tempo_config::ConfigCondition::kParseError));
    TU_LOG_INFO << readConfigResult.getStatus();
}

TEST(ConfigListenerErrorHandling, ErrorExtraCommaBetweenArrayElements)
{
    auto readConfigResult = tempo_config::read_config_string(R"(
[ 1, 2,, 3 ]
    )");
    ASSERT_THAT (readConfigResult, tempo_test::ContainsStatus(tempo_config::ConfigCondition::kParseError));
    TU_LOG_INFO << readConfigResult.getStatus();
}

TEST(ConfigListenerErrorHandling, ErrorMissingCommaBetweenArrayElements)
{
    auto readConfigResult = tempo_config::read_config_string(R"(
        ["foo" "bar"]
    )");
    ASSERT_THAT (readConfigResult, tempo_test::ContainsStatus(tempo_config::ConfigCondition::kParseError));
    TU_LOG_INFO << readConfigResult.getStatus();
}

TEST(ConfigListenerErrorHandling, ErrorMissingClosingCurly)
{
    auto readConfigResult = tempo_config::read_config_string(R"(
{
    "key1": {
        "subkey1": true
}
    )");
    ASSERT_THAT (readConfigResult, tempo_test::ContainsStatus(tempo_config::ConfigCondition::kParseError));
    TU_LOG_INFO << readConfigResult.getStatus();
}

TEST(ConfigListenerErrorHandling, ErrorExtraClosingCurly)
{
    auto readConfigResult = tempo_config::read_config_string(R"(
{
    "key1": {
        "subkey1": true
    } }
}
    )");
    ASSERT_THAT (readConfigResult, tempo_test::ContainsStatus(tempo_config::ConfigCondition::kParseError));
    TU_LOG_INFO << readConfigResult.getStatus();
}

TEST(ConfigListenerErrorHandling, ErrorLeadingCommaBeforeFirstObjectMember)
{
    auto readConfigResult = tempo_config::read_config_string(R"(
{
    "key1": 1,
    "key2": 2,,
    "key3": 3
}
    )");
    ASSERT_THAT (readConfigResult, tempo_test::ContainsStatus(tempo_config::ConfigCondition::kParseError));
    TU_LOG_INFO << readConfigResult.getStatus();
}

TEST(ConfigListenerErrorHandling, ErrorExtraCommaBetweenObjectMembers)
{
    auto readConfigResult = tempo_config::read_config_string(R"(
{
    "key1": 1,
    "key2": 2,,
    "key3": 3
}
    )");
    ASSERT_THAT (readConfigResult, tempo_test::ContainsStatus(tempo_config::ConfigCondition::kParseError));
    TU_LOG_INFO << readConfigResult.getStatus();
}

TEST(ConfigListenerErrorHandling, ErrorMissingCommaBetweenObjectMembers)
{
    auto readConfigResult = tempo_config::read_config_string(R"(
        {"foo": 1 "bar": 2}
    )");
    ASSERT_THAT (readConfigResult, tempo_test::ContainsStatus(tempo_config::ConfigCondition::kParseError));
    TU_LOG_INFO << readConfigResult.getStatus();
}

TEST(ConfigListenerErrorHandling, ErrorMissingColonBetweenMemberKeyAndValue)
{
    auto readConfigResult = tempo_config::read_config_string(R"(
{
    "key1": 1,
    "key2" 2
}
    )");
    ASSERT_THAT (readConfigResult, tempo_test::ContainsStatus(tempo_config::ConfigCondition::kParseError));
    TU_LOG_INFO << readConfigResult.getStatus();
}

TEST(ConfigListenerErrorHandling, ErrorNullIsInvalidMemberKey)
{
    auto readConfigResult = tempo_config::read_config_string(R"(
{
    null: "key1"
}
    )");
    ASSERT_THAT (readConfigResult, tempo_test::ContainsStatus(tempo_config::ConfigCondition::kParseError));
    TU_LOG_INFO << readConfigResult.getStatus();
}

TEST(ConfigListenerErrorHandling, ErrorNumberIsInvalidMemberKey)
{
    auto readConfigResult = tempo_config::read_config_string(R"(
{
    1: "key1"
}
    )");
    ASSERT_THAT (readConfigResult, tempo_test::ContainsStatus(tempo_config::ConfigCondition::kParseError));
    TU_LOG_INFO << readConfigResult.getStatus();
}
