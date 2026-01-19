#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_config/config_editor.h>

#include "tempo_test/tempo_test.h"

TEST(ConfigEditor, ParseInputAndWriteOutput)
{
    tempo_config::ConfigEditor editor;

    auto input = std::string(R"({"foo": /* comment */ "bar"})");
    ASSERT_THAT (editor.parse(input), tempo_test::IsOk());

    auto output = editor.toString();
    TU_CONSOLE_OUT << output;
    ASSERT_EQ (input, output);
}

TEST(ConfigEditor, RemoveFirstArrayElement)
{
    tempo_config::ConfigEditor editor;

    auto input = std::string(R"(["foo","bar","baz"])");
    ASSERT_THAT (editor.parse(input), tempo_test::IsOk());

    tempo_config::ConfigPath root;
    ASSERT_THAT (editor.removeNode(root.traverse(0)), tempo_test::IsOk());

    auto output = editor.toString();
    TU_CONSOLE_OUT << output;

    auto expected = std::string(R"(["bar","baz"])");
    ASSERT_EQ (expected, output);
}

TEST(ConfigEditor, RemoveLastArrayElement)
{
    tempo_config::ConfigEditor editor;

    auto input = std::string(R"(["foo","bar","baz"])");
    ASSERT_THAT (editor.parse(input), tempo_test::IsOk());

    tempo_config::ConfigPath root;
    ASSERT_THAT (editor.removeNode(root.traverse(2)), tempo_test::IsOk());

    auto output = editor.toString();
    TU_CONSOLE_OUT << output;

    auto expected = std::string(R"(["foo","bar"])");
    ASSERT_EQ (expected, output);
}
