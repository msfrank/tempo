#include <gtest/gtest.h>
#include <gmock/gmock.h>

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
    auto &token1 = tokens.at(0);
    ASSERT_EQ (tempo_command::TokenType::ARGUMENT, token1.getType());
    ASSERT_EQ ("arg0", token1.getValue());
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
    auto &token1 = tokens.at(0);
    ASSERT_EQ (tempo_command::TokenType::SHORT_OPT, token1.getType());
    ASSERT_EQ ("-v", token1.getValue());
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
    auto &token1 = tokens.at(0);
    ASSERT_EQ (tempo_command::TokenType::SHORT_OPT, token1.getType());
    ASSERT_EQ ("-a", token1.getValue());
    auto &token2 = tokens.at(1);
    ASSERT_EQ (tempo_command::TokenType::SHORT_OPT, token2.getType());
    ASSERT_EQ ("-b", token2.getValue());
    auto &token3 = tokens.at(2);
    ASSERT_EQ (tempo_command::TokenType::SHORT_OPT, token3.getType());
    ASSERT_EQ ("-c", token3.getValue());
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
    auto &token1 = tokens.at(0);
    ASSERT_EQ (tempo_command::TokenType::LONG_OPT, token1.getType());
    ASSERT_EQ ("--verbose", token1.getValue());
}

TEST(CommandTokenizer, TokenizeLongOptionWithValue)
{
    const char *argv[] = {
        "--path=/foo/bar/baz",
        nullptr,
    };

    auto tokenizeResult = tempo_command::tokenize_argv(1, argv);
    ASSERT_TRUE (tokenizeResult.isResult());
    auto tokens = tokenizeResult.getResult();

    ASSERT_EQ (2, tokens.size());
    auto &token1 = tokens.at(0);
    ASSERT_EQ (tempo_command::TokenType::LONG_OPT, token1.getType());
    ASSERT_EQ ("--path", token1.getValue());
    auto &token2 = tokens.at(1);
    ASSERT_EQ (tempo_command::TokenType::ARGUMENT, token2.getType());
    ASSERT_EQ ("/foo/bar/baz", token2.getValue());
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
    auto &token1 = tokens.at(0);
    ASSERT_EQ (tempo_command::TokenType::ARGUMENT, token1.getType());
    ASSERT_EQ ("-", token1.getValue());
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
    auto &token1 = tokens.at(0);
    ASSERT_EQ (tempo_command::TokenType::OPT_END, token1.getType());
    ASSERT_EQ ("--", token1.getValue());
}
