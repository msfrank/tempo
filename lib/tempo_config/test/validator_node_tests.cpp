#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_config/config_builder.h>
#include <tempo_config/config_utils.h>
#include <tempo_config/validator_node.h>
#include <tempo_test/tempo_test.h>

class ValidatorNode : public ::testing::Test {};

TEST_F(ValidatorNode, MatchAny)
{
    auto any = std::make_shared<tempo_config::ValidateAny>();
    ASSERT_THAT (any->validate(tempo_config::ConfigNil()), tempo_test::IsOk());
    ASSERT_THAT (any->validate(tempo_config::ConfigValue()), tempo_test::IsOk());
    ASSERT_THAT (any->validate(tempo_config::ConfigSeq()), tempo_test::IsOk());
    ASSERT_THAT (any->validate(tempo_config::ConfigMap()), tempo_test::IsOk());
}

TEST_F(ValidatorNode, MatchInteger)
{
    auto anyInteger = std::make_shared<tempo_config::ValidateInteger>();

    // positive tests
    ASSERT_THAT (anyInteger->validate(tempo_config::valueNode("0")), tempo_test::IsOk());
    ASSERT_THAT (anyInteger->validate(tempo_config::valueNode("42")), tempo_test::IsOk());
    ASSERT_THAT (anyInteger->validate(tempo_config::valueNode("-1000")), tempo_test::IsOk());
    ASSERT_THAT (anyInteger->validate(tempo_config::valueCat(std::numeric_limits<tu_int64>::min())), tempo_test::IsOk());
    ASSERT_THAT (anyInteger->validate(tempo_config::valueCat(std::numeric_limits<tu_int64>::max())), tempo_test::IsOk());

    // negative tests
    ASSERT_THAT (anyInteger->validate(tempo_config::ConfigNil()), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (anyInteger->validate(tempo_config::ConfigSeq()), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (anyInteger->validate(tempo_config::ConfigMap()), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (anyInteger->validate(tempo_config::valueNode("xyz")), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (anyInteger->validate(tempo_config::valueNode("0.123")), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (anyInteger->validate(tempo_config::valueNode("!@#$%*(^")), testing::Not(tempo_test::IsOk()));
}

TEST_F(ValidatorNode, MatchFloat)
{
    auto anyFloat = std::make_shared<tempo_config::ValidateFloat>();

    // positive tests
    ASSERT_THAT (anyFloat->validate(tempo_config::valueNode("0")), tempo_test::IsOk());
    ASSERT_THAT (anyFloat->validate(tempo_config::valueNode("0.123")), tempo_test::IsOk());
    ASSERT_THAT (anyFloat->validate(tempo_config::valueNode("-1000.678")), tempo_test::IsOk());
    ASSERT_THAT (anyFloat->validate(tempo_config::valueCat(std::numeric_limits<double>::min())), tempo_test::IsOk());
    ASSERT_THAT (anyFloat->validate(tempo_config::valueCat(std::numeric_limits<double>::max())), tempo_test::IsOk());
    ASSERT_THAT (anyFloat->validate(tempo_config::valueNode("42")), tempo_test::IsOk());
    ASSERT_THAT (anyFloat->validate(tempo_config::valueNode("-1000")), tempo_test::IsOk());

    // negative tests
    ASSERT_THAT (anyFloat->validate(tempo_config::ConfigNil()), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (anyFloat->validate(tempo_config::ConfigSeq()), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (anyFloat->validate(tempo_config::ConfigMap()), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (anyFloat->validate(tempo_config::valueNode("xyz")), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (anyFloat->validate(tempo_config::valueNode("!@#$%*(^")), testing::Not(tempo_test::IsOk()));
}

TEST_F(ValidatorNode, MatchString)
{
    auto anyString = std::make_shared<tempo_config::ValidateString>();

    // positive tests
    ASSERT_THAT (anyString->validate(tempo_config::valueNode("")), tempo_test::IsOk());
    ASSERT_THAT (anyString->validate(tempo_config::valueNode("hello, world!")), tempo_test::IsOk());
    ASSERT_THAT (anyString->validate(tempo_config::valueNode("!@#$%*(^")), tempo_test::IsOk());
    ASSERT_THAT (anyString->validate(tempo_config::valueNode("true")), tempo_test::IsOk());
    ASSERT_THAT (anyString->validate(tempo_config::valueNode("false")), tempo_test::IsOk());
    ASSERT_THAT (anyString->validate(tempo_config::valueNode("null")), tempo_test::IsOk());
    ASSERT_THAT (anyString->validate(tempo_config::valueNode("0")), tempo_test::IsOk());
    ASSERT_THAT (anyString->validate(tempo_config::valueNode("42")), tempo_test::IsOk());
    ASSERT_THAT (anyString->validate(tempo_config::valueNode("-1000")), tempo_test::IsOk());

    // negative tests
    ASSERT_THAT (anyString->validate(tempo_config::ConfigNil()), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (anyString->validate(tempo_config::ConfigSeq()), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (anyString->validate(tempo_config::ConfigMap()), testing::Not(tempo_test::IsOk()));
}

TEST_F(ValidatorNode, MatchAnyOf)
{
    std::vector<std::shared_ptr<ValidatorNode>> validators;

    auto multipleOfThree = std::make_shared<tempo_config::ValidateInteger>(
        std::numeric_limits<tu_int64>::min(), std::numeric_limits<tu_int64>::max(), 3);
    auto multipleOfFive = std::make_shared<tempo_config::ValidateInteger>(
        std::numeric_limits<tu_int64>::min(), std::numeric_limits<tu_int64>::max(), 5);
    auto positiveAndLessThan100 = std::make_shared<tempo_config::ValidateInteger>(
        0, 100, 1, false, true);

    auto val1 = std::static_pointer_cast<tempo_config::ValidatorNode>(multipleOfThree);
    auto val2 = std::static_pointer_cast<tempo_config::ValidatorNode>(multipleOfFive);
    auto val3 = std::static_pointer_cast<tempo_config::ValidatorNode>(positiveAndLessThan100);
    auto anyOf = std::make_shared<tempo_config::ValidateAnyOf>(std::initializer_list{ val1, val2, val3 });

    ASSERT_THAT (anyOf->validate(tempo_config::valueNode("3")), tempo_test::IsOk());
    ASSERT_THAT (anyOf->validate(tempo_config::valueNode("5")), tempo_test::IsOk());
    ASSERT_THAT (anyOf->validate(tempo_config::valueNode("99")), tempo_test::IsOk());
    ASSERT_THAT (anyOf->validate(tempo_config::valueNode("333")), tempo_test::IsOk());
    ASSERT_THAT (anyOf->validate(tempo_config::valueNode("55555")), tempo_test::IsOk());

    // negative tests
    ASSERT_THAT (anyOf->validate(tempo_config::ConfigNil()), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (anyOf->validate(tempo_config::ConfigSeq()), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (anyOf->validate(tempo_config::ConfigMap()), testing::Not(tempo_test::IsOk()));
}

TEST_F(ValidatorNode, MatchAllOf)
{
    std::vector<std::shared_ptr<ValidatorNode>> validators;

    auto multipleOfThree = std::make_shared<tempo_config::ValidateInteger>(
        std::numeric_limits<tu_int64>::min(), std::numeric_limits<tu_int64>::max(), 3);
    auto multipleOfFive = std::make_shared<tempo_config::ValidateInteger>(
        std::numeric_limits<tu_int64>::min(), std::numeric_limits<tu_int64>::max(), 5);
    auto positiveAndLessThan100 = std::make_shared<tempo_config::ValidateInteger>(
        0, 100, 1, false, true);

    auto val1 = std::static_pointer_cast<tempo_config::ValidatorNode>(multipleOfThree);
    auto val2 = std::static_pointer_cast<tempo_config::ValidatorNode>(multipleOfFive);
    auto val3 = std::static_pointer_cast<tempo_config::ValidatorNode>(positiveAndLessThan100);
    auto allOf = std::make_shared<tempo_config::ValidateAllOf>(std::initializer_list{ val1, val2, val3 });

    ASSERT_THAT (allOf->validate(tempo_config::valueNode("15")), tempo_test::IsOk());
    ASSERT_THAT (allOf->validate(tempo_config::valueNode("45")), tempo_test::IsOk());
    ASSERT_THAT (allOf->validate(tempo_config::valueNode("90")), tempo_test::IsOk());

    // negative tests
    ASSERT_THAT (allOf->validate(tempo_config::ConfigNil()), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (allOf->validate(tempo_config::ConfigSeq()), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (allOf->validate(tempo_config::ConfigMap()), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (allOf->validate(tempo_config::valueNode("3")), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (allOf->validate(tempo_config::valueNode("5")), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (allOf->validate(tempo_config::valueNode("99")), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (allOf->validate(tempo_config::valueNode("333")), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (allOf->validate(tempo_config::valueNode("55555")), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (allOf->validate(tempo_config::valueNode("-15")), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (allOf->validate(tempo_config::valueNode("-45")), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (allOf->validate(tempo_config::valueNode("-90")), testing::Not(tempo_test::IsOk()));
}

TEST_F(ValidatorNode, MatchNot)
{
    auto anyInteger = std::make_shared<tempo_config::ValidateInteger>();
    auto notInteger = std::make_shared<tempo_config::ValidateNot>(anyInteger);

    // positive tests
    ASSERT_THAT (notInteger->validate(tempo_config::ConfigNil()), tempo_test::IsOk());
    ASSERT_THAT (notInteger->validate(tempo_config::ConfigValue()), tempo_test::IsOk());
    ASSERT_THAT (notInteger->validate(tempo_config::ConfigSeq()), tempo_test::IsOk());
    ASSERT_THAT (notInteger->validate(tempo_config::ConfigMap()), tempo_test::IsOk());
    ASSERT_THAT (notInteger->validate(tempo_config::valueNode("xyz")), tempo_test::IsOk());
    ASSERT_THAT (notInteger->validate(tempo_config::valueNode("0.123")), tempo_test::IsOk());
    ASSERT_THAT (notInteger->validate(tempo_config::valueNode("!@#$%*(^")), tempo_test::IsOk());

    // negative tests
    ASSERT_THAT (notInteger->validate(tempo_config::valueNode("0")), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (notInteger->validate(tempo_config::valueNode("42")), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (notInteger->validate(tempo_config::valueNode("-1000")), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (notInteger->validate(tempo_config::valueCat(std::numeric_limits<tu_int64>::min())), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (notInteger->validate(tempo_config::valueCat(std::numeric_limits<tu_int64>::max())), testing::Not(tempo_test::IsOk()));
}
