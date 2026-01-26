#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_config/config_builder.h>
#include <tempo_config/config_utils.h>
#include <tempo_config/validator_node.h>
#include <tempo_test/tempo_test.h>

class ValidatorNode : public ::testing::Test {};

TEST_F(ValidatorNode, ValidateAny)
{
    auto any = std::make_shared<tempo_config::ValidateAny>();
    ASSERT_THAT (any->validate(tempo_config::ConfigNil()), tempo_test::IsOk());
    ASSERT_THAT (any->validate(tempo_config::ConfigValue()), tempo_test::IsOk());
    ASSERT_THAT (any->validate(tempo_config::ConfigSeq()), tempo_test::IsOk());
    ASSERT_THAT (any->validate(tempo_config::ConfigMap()), tempo_test::IsOk());
}

TEST_F(ValidatorNode, ValidateInteger)
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

TEST_F(ValidatorNode, ValidateFloat)
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

TEST_F(ValidatorNode, ValidateString)
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

TEST_F(ValidatorNode, ValidateSeq)
{
    auto anySeq = std::make_shared<tempo_config::ValidateSeq>();

    // positive tests
    ASSERT_THAT (anySeq->validate(tempo_config::ConfigSeq()), tempo_test::IsOk());
    ASSERT_THAT (anySeq->validate(
        tempo_config::startSeq()
            .append(tempo_config::valueNode("1"))
            .buildNode()),
        tempo_test::IsOk());
    ASSERT_THAT (anySeq->validate(
        tempo_config::startSeq()
            .append(tempo_config::valueNode("1"))
            .append(tempo_config::valueNode("2"))
            .append(tempo_config::valueNode("3"))
            .buildNode()),
        tempo_test::IsOk());

    // negative tests
    ASSERT_THAT (anySeq->validate(tempo_config::ConfigNil()), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (anySeq->validate(tempo_config::ConfigValue()), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (anySeq->validate(tempo_config::ConfigMap()), testing::Not(tempo_test::IsOk()));
}

TEST_F(ValidatorNode, ValidateMap)
{
    auto anyMap = std::make_shared<tempo_config::ValidateMap>();

    // positive tests
    ASSERT_THAT (anyMap->validate(tempo_config::ConfigMap()), tempo_test::IsOk());
    ASSERT_THAT (anyMap->validate(
        tempo_config::startMap()
            .put("one", tempo_config::valueNode("1"))
            .buildNode()),
        tempo_test::IsOk());
    ASSERT_THAT (anyMap->validate(
        tempo_config::startMap()
            .put("one", tempo_config::valueNode("1"))
            .put("two", tempo_config::valueNode("2"))
            .put("three", tempo_config::valueNode("3"))
            .buildNode()),
        tempo_test::IsOk());

    // negative tests
    ASSERT_THAT (anyMap->validate(tempo_config::ConfigNil()), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (anyMap->validate(tempo_config::ConfigValue()), testing::Not(tempo_test::IsOk()));
    ASSERT_THAT (anyMap->validate(tempo_config::ConfigSeq()), testing::Not(tempo_test::IsOk()));
}

TEST_F(ValidatorNode, ValidateAnyOf)
{
    auto multipleOfThree = std::make_shared<tempo_config::ValidateInteger>(
        std::numeric_limits<tu_int64>::min(), std::numeric_limits<tu_int64>::max(), 3);
    auto multipleOfFive = std::make_shared<tempo_config::ValidateInteger>(
        std::numeric_limits<tu_int64>::min(), std::numeric_limits<tu_int64>::max(), 5);
    auto positiveAndLessThan100 = std::make_shared<tempo_config::ValidateInteger>(
        0, 100, 1, false, true);

    std::vector<std::shared_ptr<tempo_config::ValidatorNode>> validators;
    validators.push_back(multipleOfThree);
    validators.push_back(multipleOfFive);
    validators.push_back(positiveAndLessThan100);
    auto anyOf = std::make_shared<tempo_config::ValidateAnyOf>(std::move(validators));

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

TEST_F(ValidatorNode, ValidateAllOf)
{
    auto multipleOfThree = std::make_shared<tempo_config::ValidateInteger>(
        std::numeric_limits<tu_int64>::min(), std::numeric_limits<tu_int64>::max(), 3);
    auto multipleOfFive = std::make_shared<tempo_config::ValidateInteger>(
        std::numeric_limits<tu_int64>::min(), std::numeric_limits<tu_int64>::max(), 5);
    auto positiveAndLessThan100 = std::make_shared<tempo_config::ValidateInteger>(
        0, 100, 1, false, true);

    std::vector<std::shared_ptr<tempo_config::ValidatorNode>> validators;
    validators.push_back(multipleOfThree);
    validators.push_back(multipleOfFive);
    validators.push_back(positiveAndLessThan100);
    auto allOf = std::make_shared<tempo_config::ValidateAllOf>(std::move(validators));

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

TEST_F(ValidatorNode, ValidateNot)
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
