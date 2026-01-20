#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_config/config_editor.h>

#include "tempo_config/config_builder.h"
#include "tempo_test/tempo_test.h"

class ConfigEditor : public ::testing::Test {};

TEST_F(ConfigEditor, ParseInputAndWriteOutput)
{
    tempo_config::ConfigEditor editor;

    auto input = std::string(R"({"foo": /* comment */ "1"})");
    ASSERT_THAT (editor.parse(input), tempo_test::IsOk());

    auto output = editor.toString();
    TU_CONSOLE_OUT << output;
    ASSERT_EQ (input, output);
}

TEST_F(ConfigEditor, RemoveFirstArrayElement)
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

TEST_F(ConfigEditor, RemoveMiddleArrayElement)
{
    tempo_config::ConfigEditor editor;

    auto input = std::string(R"(["foo","bar","baz"])");
    ASSERT_THAT (editor.parse(input), tempo_test::IsOk());

    tempo_config::ConfigPath root;
    ASSERT_THAT (editor.removeNode(root.traverse(1)), tempo_test::IsOk());

    auto output = editor.toString();
    TU_CONSOLE_OUT << output;

    auto expected = std::string(R"(["foo","baz"])");
    ASSERT_EQ (expected, output);
}

TEST_F(ConfigEditor, RemoveLastArrayElement)
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

TEST_F(ConfigEditor, RemoveFirstObjectMember)
{
    tempo_config::ConfigEditor editor;

    auto input = std::string(R"({"foo": "1", "bar": "2", "baz": "3"})");
    ASSERT_THAT (editor.parse(input), tempo_test::IsOk());

    tempo_config::ConfigPath root;
    ASSERT_THAT (editor.removeNode(root.traverse("foo")), tempo_test::IsOk());

    auto output = editor.toString();
    TU_CONSOLE_OUT << output;

    auto expected = std::string(R"({ "bar": "2", "baz": "3"})");
    ASSERT_EQ (expected, output);
}

TEST_F(ConfigEditor, RemoveMiddleObjectMember)
{
    tempo_config::ConfigEditor editor;

    auto input = std::string(R"({"foo": "1", "bar": "2", "baz": "3"})");
    ASSERT_THAT (editor.parse(input), tempo_test::IsOk());

    tempo_config::ConfigPath root;
    ASSERT_THAT (editor.removeNode(root.traverse("bar")), tempo_test::IsOk());

    auto output = editor.toString();
    TU_CONSOLE_OUT << output;

    auto expected = std::string(R"({"foo": "1", "baz": "3"})");
    ASSERT_EQ (expected, output);
}

TEST_F(ConfigEditor, RemoveLastObjectMember)
{
    tempo_config::ConfigEditor editor;

    auto input = std::string(R"({"foo": "1", "bar": "2", "baz": "3"})");
    ASSERT_THAT (editor.parse(input), tempo_test::IsOk());

    tempo_config::ConfigPath root;
    ASSERT_THAT (editor.removeNode(root.traverse("baz")), tempo_test::IsOk());

    auto output = editor.toString();
    TU_CONSOLE_OUT << output;

    auto expected = std::string(R"({"foo": "1", "bar": "2"})");
    ASSERT_EQ (expected, output);
}

TEST_F(ConfigEditor, InsertNodeAtArrayStart)
{
    tempo_config::ConfigEditor editor;

    auto input = std::string(R"(["bar","baz"])");
    ASSERT_THAT (editor.parse(input), tempo_test::IsOk());

    tempo_config::ConfigPath root;
    ASSERT_THAT (editor.insertNode(root.traverse(0), tempo_config::valueNode("qux")), tempo_test::IsOk());

    auto output = editor.toString();
    TU_CONSOLE_OUT << output;

    auto expected = std::string(R"(["qux","bar","baz"])");
    ASSERT_EQ (expected, output);
}

TEST_F(ConfigEditor, InsertNodeBeforeLastArrayElement)
{
    tempo_config::ConfigEditor editor;

    auto input = std::string(R"(["bar","baz"])");
    ASSERT_THAT (editor.parse(input), tempo_test::IsOk());

    tempo_config::ConfigPath root;
    ASSERT_THAT (editor.insertNode(root.traverse(1), tempo_config::valueNode("qux")), tempo_test::IsOk());

    auto output = editor.toString();
    TU_CONSOLE_OUT << output;

    auto expected = std::string(R"(["bar","qux","baz"])");
    ASSERT_EQ (expected, output);
}

TEST_F(ConfigEditor, InsertNodeInEmptyObject)
{
    tempo_config::ConfigEditor editor;

    auto input = std::string(R"({})");
    ASSERT_THAT (editor.parse(input), tempo_test::IsOk());

    tempo_config::ConfigPath root;
    ASSERT_THAT (editor.insertNode(root.traverse("foo"), tempo_config::valueNode("1")), tempo_test::IsOk());

    auto output = editor.toString();
    TU_CONSOLE_OUT << output;

    auto expected = std::string(R"({"foo":"1"})");
    ASSERT_EQ (expected, output);
}

TEST_F(ConfigEditor, InsertNodeAtTheEndOfTheObject)
{
    tempo_config::ConfigEditor editor;

    auto input = std::string(R"({"foo": "1"})");
    ASSERT_THAT (editor.parse(input), tempo_test::IsOk());

    tempo_config::ConfigPath root;
    ASSERT_THAT (editor.insertNode(root.traverse("bar"), tempo_config::valueNode("2")), tempo_test::IsOk());

    auto output = editor.toString();
    TU_CONSOLE_OUT << output;

    auto expected = std::string(R"({"foo": "1","bar":"2"})");
    ASSERT_EQ (expected, output);
}
