
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <tempo_command/command_tokenizer.h>

TEST(CommandTokenizer, TokenizeArgument)
{
    const char *argv[] = {
        "arg0",
        nullptr,
    };

    auto tokenizeResult = tempo_command::tokenize_argv(1, argv);
    ASSERT_TRUE (tokenizeResult.isResult());
    auto tokens = tokenizeResult.getResult();

    ASSERT_EQ (1, tokens.size());
    auto token1 = tokens.at(0);
    ASSERT_EQ (tempo_command::TokenType::ARGUMENT, token1.type);
    ASSERT_EQ ("arg0", token1.value);
}

TEST(CommandTokenizer, TokenizeShortOption)
{
    const char *argv[] = {
        "-v",
        nullptr,
    };

    auto tokenizeResult = tempo_command::tokenize_argv(1, argv);
    ASSERT_TRUE (tokenizeResult.isResult());
    auto tokens = tokenizeResult.getResult();

    ASSERT_EQ (1, tokens.size());
    auto token1 = tokens.at(0);
    ASSERT_EQ (tempo_command::TokenType::SHORT_OPT, token1.type);
    ASSERT_EQ ("-v", token1.value);
}

TEST(CommandTokenizer, TokenizeRunOfShortOptions)
{
    const char *argv[] = {
        "-abc",
        nullptr,
    };

    auto tokenizeResult = tempo_command::tokenize_argv(1, argv);
    ASSERT_TRUE (tokenizeResult.isResult());
    auto tokens = tokenizeResult.getResult();

    ASSERT_EQ (3, tokens.size());
    auto token1 = tokens.at(0);
    ASSERT_EQ (tempo_command::TokenType::SHORT_OPT, token1.type);
    ASSERT_EQ ("-a", token1.value);
    auto token2 = tokens.at(1);
    ASSERT_EQ (tempo_command::TokenType::SHORT_OPT, token2.type);
    ASSERT_EQ ("-b", token2.value);
    auto token3 = tokens.at(2);
    ASSERT_EQ (tempo_command::TokenType::SHORT_OPT, token3.type);
    ASSERT_EQ ("-c", token3.value);
}

TEST(CommandTokenizer, TokenizeLongOption)
{
    const char *argv[] = {
        "--verbose",
        nullptr,
    };

    auto tokenizeResult = tempo_command::tokenize_argv(1, argv);
    ASSERT_TRUE (tokenizeResult.isResult());
    auto tokens = tokenizeResult.getResult();

    ASSERT_EQ (1, tokens.size());
    auto token1 = tokens.at(0);
    ASSERT_EQ (tempo_command::TokenType::LONG_OPT, token1.type);
    ASSERT_EQ ("--verbose", token1.value);
}

TEST(CommandTokenizer, TokenizeSingleDashAsArgument)
{
    const char *argv[] = {
        "-",
        nullptr,
    };

    auto tokenizeResult = tempo_command::tokenize_argv(1, argv);
    ASSERT_TRUE (tokenizeResult.isResult());
    auto tokens = tokenizeResult.getResult();

    ASSERT_EQ (1, tokens.size());
    auto token1 = tokens.at(0);
    ASSERT_EQ (tempo_command::TokenType::ARGUMENT, token1.type);
    ASSERT_EQ ("-", token1.value);
}

TEST(CommandTokenizer, TokenizeOptionsEnd)
{
    const char *argv[] = {
        "--",
        nullptr,
    };

    auto tokenizeResult = tempo_command::tokenize_argv(1, argv);
    ASSERT_TRUE (tokenizeResult.isResult());
    auto tokens = tokenizeResult.getResult();

    ASSERT_EQ (1, tokens.size());
    auto token1 = tokens.at(0);
    ASSERT_EQ (tempo_command::TokenType::OPT_END, token1.type);
    ASSERT_EQ ("--", token1.value);
}
