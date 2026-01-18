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