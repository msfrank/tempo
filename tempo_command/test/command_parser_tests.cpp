
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <tempo_command/command_config.h>
#include <tempo_command/command_parser.h>

TEST(CommandParser, ParseCompletelyArgument)
{
    const char *argv[] = {
        "arg0",
        nullptr,
    };

    auto tokenizeResult = tempo_command::tokenize_argv(1, argv);
    ASSERT_TRUE (tokenizeResult.isResult());
    auto tokens = tokenizeResult.getResult();

    std::vector<tempo_command::Grouping> groupings = {
        {"verbose", {"-v", "--verbose"}, tempo_command::GroupingType::NO_ARGUMENT},
    };

    std::vector<tempo_command::Mapping> optMappings = {
        {tempo_command::MappingType::ANY_INSTANCES, "verbose"},
    };

    std::vector<tempo_command::Mapping> argMappings = {
        {tempo_command::MappingType::ANY_INSTANCES, "arg"},
    };

    tempo_command::OptionsHash options;
    tempo_command::ArgumentVector arguments;

    auto status = tempo_command::parse_completely(tokens, groupings, options, arguments);
    ASSERT_TRUE (status.isOk());

    ASSERT_EQ (0, options.size());

    ASSERT_EQ (1, arguments.size());
    ASSERT_EQ ("arg0", arguments.at(0));
}

TEST(CommandParser, ParseCompletelyShortOption)
{
    const char *argv[] = {
        "-v",
        nullptr,
    };

    auto tokenizeResult = tempo_command::tokenize_argv(1, argv);
    ASSERT_TRUE (tokenizeResult.isResult());
    auto tokens = tokenizeResult.getResult();

    std::vector<tempo_command::Grouping> groupings = {
        {"verbose", {"-v", "--verbose"}, tempo_command::GroupingType::NO_ARGUMENT},
    };

    std::vector<tempo_command::Mapping> optMappings = {
        {tempo_command::MappingType::ANY_INSTANCES, "verbose"},
    };

    std::vector<tempo_command::Mapping> argMappings;

    tempo_command::OptionsHash options;
    tempo_command::ArgumentVector arguments;

    auto status = tempo_command::parse_completely(tokens, groupings, options, arguments);
    ASSERT_TRUE (status.isOk());

    ASSERT_EQ (1, options.size());
    auto verbose = options.at("verbose");
    ASSERT_EQ (1, verbose.size());

    ASSERT_EQ (0, arguments.size());
}

TEST(CommandParser, ParseCompletelyRunOfShortOptions)
{
    const char *argv[] = {
        "-vvv",
        nullptr,
    };

    auto tokenizeResult = tempo_command::tokenize_argv(1, argv);
    ASSERT_TRUE (tokenizeResult.isResult());
    auto tokens = tokenizeResult.getResult();

    std::vector<tempo_command::Grouping> groupings = {
        {"verbose", {"-v", "--verbose"}, tempo_command::GroupingType::NO_ARGUMENT},
    };

    std::vector<tempo_command::Mapping> optMappings = {
        {tempo_command::MappingType::ANY_INSTANCES, "verbose"},
    };

    std::vector<tempo_command::Mapping> argMappings;

    tempo_command::OptionsHash options;
    tempo_command::ArgumentVector arguments;

    auto status = tempo_command::parse_completely(tokens, groupings, options, arguments);
    ASSERT_TRUE (status.isOk());

    ASSERT_EQ (1, options.size());
    auto verbose = options.at("verbose");
    ASSERT_EQ (3, verbose.size());

    ASSERT_EQ (0, arguments.size());
}

TEST(CommandParser, ParseCompletelyLongOption)
{
    const char *argv[] = {
        "--verbose",
        nullptr,
    };

    auto tokenizeResult = tempo_command::tokenize_argv(1, argv);
    ASSERT_TRUE (tokenizeResult.isResult());
    auto tokens = tokenizeResult.getResult();

    std::vector<tempo_command::Grouping> groupings = {
        {"verbose", {"-v", "--verbose"}, tempo_command::GroupingType::NO_ARGUMENT},
    };

    std::vector<tempo_command::Mapping> optMappings = {
        {tempo_command::MappingType::ANY_INSTANCES, "verbose"},
    };

    std::vector<tempo_command::Mapping> argMappings;

    tempo_command::OptionsHash options;
    tempo_command::ArgumentVector arguments;

    auto status = tempo_command::parse_completely(tokens, groupings, options, arguments);
    ASSERT_TRUE (status.isOk());

    ASSERT_EQ (1, options.size());
    auto verbose = options.at("verbose");
    ASSERT_EQ (1, verbose.size());

    ASSERT_EQ (0, arguments.size());
}
