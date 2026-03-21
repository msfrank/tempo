#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_config/config_builder.h>
#include <tempo_config/config_editor.h>
#include <tempo_test/tempo_test.h>

#include <tempo_config/config_file_editor.h>
#include <tempo_config/config_utils.h>
#include <tempo_utils/tempfile_maker.h>

class ConfigFileEditor : public ::testing::Test {
protected:
    std::unique_ptr<tempo_utils::TempfileMaker> configFile;
    void SetUp() override {
        tempo_config::ConfigMap emptyMap;
        configFile = std::make_unique<tempo_utils::TempfileMaker>(
            std::filesystem::current_path(), "config.XXXXXXXX", emptyMap.toString());
        TU_RAISE_IF_NOT_OK (configFile->getStatus());
    }
    void TearDown() override {
        auto configFilePath = configFile->getTempfile();
        if (std::filesystem::exists(configFilePath)) {
            std::filesystem::remove(configFilePath);
        }
    }
};

TEST_F(ConfigFileEditor, InsertNodeAndWriteOutput)
{
    auto configFilePath = configFile->getTempfile();

    tempo_config::ConfigFileEditor fileEditor(configFilePath);
    ASSERT_THAT (fileEditor.getStatus(), tempo_test::IsOk());

    tempo_config::ConfigPath root;
    ASSERT_THAT (fileEditor.insertNode(root.traverse("foo"), tempo_config::valueNode("bar")), tempo_test::IsOk());
    ASSERT_THAT (fileEditor.writeFile(), tempo_test::IsOk());

    tempo_config::ConfigMap configMap;
    auto readResult = tempo_config::read_config_map_file(configFilePath);
    ASSERT_THAT (readResult, tempo_test::IsResult());

    auto expected = tempo_config::startMap()
        .put("foo", tempo_config::valueNode("bar"))
        .buildMap();
    auto actual = readResult.getResult();
    ASSERT_EQ (expected, actual);
}
