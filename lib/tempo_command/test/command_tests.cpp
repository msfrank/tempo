#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_command/command.h>
#include <tempo_command/command_config.h>
#include <tempo_command/command_parser.h>
#include <tempo_config/config_builder.h>
#include <tempo_test/tempo_test.h>

#include "tempo_config/base_conversions.h"

class Command : public ::testing::Test {};

TEST_F(Command, ParseCompletelyArgument)
{
    tempo_command::Command command({"test"});
    ASSERT_THAT (command.addArgument(
        "number", "NUMBER", tempo_command::MappingType::ONE_INSTANCE),
    tempo_test::IsOk());

    const char *argv[] = {
        "42",
        nullptr,
    };
    ASSERT_THAT (command.parse(1, argv), tempo_test::IsOk());

    ASSERT_EQ (1, command.configSize());
    ASSERT_EQ (tempo_config::valueNode("42"), command.getConfig("number"));

    int number;
    tempo_config::IntegerParser numberParser;
    ASSERT_THAT (command.convert(number, numberParser, "number"), tempo_test::IsOk());
    ASSERT_EQ (42, number);
}

TEST_F(Command, ParseCompletelyFlag)
{
    tempo_command::Command command({"test"});
    ASSERT_THAT (command.addFlag(
        "flag", {"-f"}, tempo_command::MappingType::TRUE_IF_INSTANCE),
    tempo_test::IsOk());

    const char *argv[] = {
        "-f",
        nullptr,
    };
    ASSERT_THAT (command.parse(1, argv), tempo_test::IsOk());

    ASSERT_EQ (1, command.configSize());
    ASSERT_EQ (tempo_config::valueNode("true"), command.getConfig("flag"));

    bool flag;
    tempo_config::BooleanParser flagParser;
    ASSERT_THAT (command.convert(flag, flagParser, "flag"), tempo_test::IsOk());
    ASSERT_TRUE (flag);
}

TEST_F(Command, ParseCompletelyOption)
{
    tempo_command::Command command({"test"});
    ASSERT_THAT (command.addOption(
        "option", {"-o"}, tempo_command::MappingType::ONE_INSTANCE),
    tempo_test::IsOk());

    const char *argv[] = {
        "-o",
        "value",
        nullptr,
    };
    ASSERT_THAT (command.parse(2, argv), tempo_test::IsOk());

    ASSERT_EQ (1, command.configSize());
    ASSERT_EQ (tempo_config::valueNode("value"), command.getConfig("option"));

    std::string option;
    tempo_config::StringParser optionParser;
    ASSERT_THAT (command.convert(option, optionParser, "option"), tempo_test::IsOk());
    ASSERT_EQ ("value", option);
}

TEST_F(Command, ParseFailsUnknownOption)
{
    tempo_command::Command command({"test"});

    const char *argv[] = {
        "-o",
        "value",
        nullptr,
    };
    auto status = command.parse(2, argv);
    TU_CONSOLE_ERR << status;
    ASSERT_THAT (status, tempo_test::IsCondition(tempo_command::CommandCondition::kUnknownCommandOption));
}

TEST_F(Command, ParseFailsOptionMissingValue)
{
    tempo_command::Command command({"test"});
    ASSERT_THAT (command.addOption(
        "option", {"-o"}, tempo_command::MappingType::ONE_INSTANCE),
    tempo_test::IsOk());

    const char *argv[] = {
        "-o",
        nullptr,
    };
    auto status = command.parse(1, argv);
    TU_CONSOLE_ERR << status;
    ASSERT_THAT (status, tempo_test::IsCondition(tempo_command::CommandCondition::kInvalidConfiguration));
}

TEST_F(Command, ConvertFailsValidationError)
{
    tempo_command::Command command({"test"});
    ASSERT_THAT (command.addOption(
        "number", {"--number"}, tempo_command::MappingType::ONE_INSTANCE),
    tempo_test::IsOk());

    const char *argv[] = {
        "--number",
        "value",
        nullptr,
    };
    ASSERT_THAT (command.parse(2, argv), tempo_test::IsOk());

    int number;
    tempo_config::IntegerParser numberParser;
    auto status = command.convert(number, numberParser, "number");
    TU_CONSOLE_ERR << status;
    ASSERT_THAT (status, tempo_test::IsCondition(tempo_command::CommandCondition::kInvalidConfiguration));
}