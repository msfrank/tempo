#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_config/config_builder.h>
#include <tempo_config/config_editor.h>
#include <tempo_config/internal/piece_store.h>
#include <tempo_test/tempo_test.h>

#include "tempo_config/config_utils.h"

class PiecePrinter : public ::testing::Test {
protected:
    void SetUp() override {
        tempo_utils::LoggingConfiguration loggingConf;
        loggingConf.severityFilter = tempo_utils::SeverityFilter::kDefault;
        tempo_utils::init_logging(loggingConf);
    }
};

TEST_F(PiecePrinter, ParseInputAndPrintOutput)
{
    tempo_config::ConfigEditor editor;

    auto input = std::string(R"({"foo": /* comment */ "1"})");
    ASSERT_THAT (editor.parse(input), tempo_test::IsOk());

    std::string output;
    ASSERT_THAT (editor.writeJson(output), tempo_test::IsOk());

    TU_CONSOLE_OUT << output;
    ASSERT_EQ (input, output);
    ASSERT_THAT (tempo_config::read_config_string(output), tempo_test::IsResult());
}

TEST_F(PiecePrinter, ParseMultiLineInputAndPrintOutput)
{
    tempo_config::ConfigEditor editor;

    auto input = std::string(R"(
{
    "foo": /* comment */ "1"
}
)");

    ASSERT_THAT (editor.parse(input), tempo_test::IsOk());

    std::string output;
    ASSERT_THAT (editor.writeJson(output), tempo_test::IsOk());

    TU_CONSOLE_OUT << output;
    ASSERT_EQ (input, output);
    ASSERT_THAT (tempo_config::read_config_string(output), tempo_test::IsResult());
}

TEST_F(PiecePrinter, AddElementToMultilineArrayAndPrintOutput)
{
    tempo_config::ConfigEditor editor;

    auto input = std::string(R"(
[
    "foo"
]
)");

    ASSERT_THAT (editor.parse(input), tempo_test::IsOk());
    editor.insertNode(tempo_config::ConfigPath().traverse(-1), tempo_config::valueNode("bar"));

    std::string output;
    ASSERT_THAT (editor.writeJson(output), tempo_test::IsOk());
    TU_CONSOLE_OUT << output;

    auto expected = std::string(R"(
[
    "foo",
    "bar"
]
)");
    ASSERT_EQ (expected, output);
    ASSERT_THAT (tempo_config::read_config_string(output), tempo_test::IsResult());
}

TEST_F(PiecePrinter, AddElementToCompactArrayAndPrintOutput)
{
    tempo_config::ConfigEditor editor;

    auto input = std::string(R"(
[ "foo" ])");

    ASSERT_THAT (editor.parse(input), tempo_test::IsOk());
    editor.insertNode(tempo_config::ConfigPath().traverse(-1), tempo_config::valueNode("bar"));

    std::string output;
    ASSERT_THAT (editor.writeJson(output), tempo_test::IsOk());
    TU_CONSOLE_OUT << output;

    auto expected = std::string(R"(
[ "foo",
"bar" ])");
    ASSERT_EQ (expected, output);
    ASSERT_THAT (tempo_config::read_config_string(output), tempo_test::IsResult());
}

TEST_F(PiecePrinter, AddMemberToMultilineObjectAndPrintOutput)
{
    tempo_config::ConfigEditor editor;

    auto input = std::string(R"(
{
    "foo": /* comment */ "1"
}
)");

    ASSERT_THAT (editor.parse(input), tempo_test::IsOk());
    editor.insertNode(tempo_config::ConfigPath().traverse("bar"), tempo_config::valueNode("2"));

    std::string output;
    ASSERT_THAT (editor.writeJson(output), tempo_test::IsOk());
    TU_CONSOLE_OUT << output;

    auto expected = std::string(R"(
{
    "foo": /* comment */ "1",
    "bar": "2"
}
)");
    ASSERT_EQ (expected, output);
    ASSERT_THAT (tempo_config::read_config_string(output), tempo_test::IsResult());
}

TEST_F(PiecePrinter, AddMemberToCompactObjectAndPrintOutput)
{
    tempo_config::ConfigEditor editor;

    auto input = std::string(R"(
{ "foo": /* comment */ "1" }
)");

    ASSERT_THAT (editor.parse(input), tempo_test::IsOk());
    editor.insertNode(tempo_config::ConfigPath().traverse("bar"), tempo_config::valueNode("2"));

    std::string output;
    ASSERT_THAT (editor.writeJson(output), tempo_test::IsOk());
    TU_CONSOLE_OUT << output;

    auto expected = std::string(R"(
{ "foo": /* comment */ "1",
"bar": "2" }
)");
    ASSERT_EQ (expected, output);
    ASSERT_THAT (tempo_config::read_config_string(output), tempo_test::IsResult());
}
