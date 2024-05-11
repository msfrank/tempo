
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <tempo_config/config_serde.h>
#include <tempo_config/config_types.h>
#include <tempo_utils/directory_maker.h>
#include <tempo_utils/file_writer.h>
#include <tempo_utils/tempdir_maker.h>

class ConfigSerde : public ::testing::Test {
protected:

    std::filesystem::path rootDirectory;

    void SetUp() override
    {
        tempo_utils::TempdirMaker tempdirMaker(std::filesystem::current_path(), "root.XXXXXXXX");
        ASSERT_TRUE (tempdirMaker.isValid());
        rootDirectory = tempdirMaker.getTempdir();
    }

    std::filesystem::path makeDirectory(const std::filesystem::path &relativePath)
    {
        tempo_utils::DirectoryMaker directoryMaker(rootDirectory, relativePath);
        TU_ASSERT (directoryMaker.isValid());
        return directoryMaker.getAbsolutePath();
    }

    std::filesystem::path writeConfigFile(
        const std::filesystem::path &relativePath,
        std::string_view contents)
    {
        tempo_utils::FileWriter fileWriter(rootDirectory / relativePath, contents, tempo_utils::FileWriterMode::CREATE_ONLY);
        TU_ASSERT (fileWriter.isValid());
        return fileWriter.getAbsolutePath();
    }

    std::filesystem::path writeConfigFile(
        const std::filesystem::path &relativePath,
        const tempo_config::ConfigNode &node)
    {
        return writeConfigFile(relativePath, node.toString());
    }

    void TearDown() override {
        std::filesystem::remove_all(rootDirectory);
    }
};

TEST_F(ConfigSerde, ReadConfigFile)
{
    auto nullConfigPath = writeConfigFile("null.config", "null");
    auto readNilResult = tempo_config::read_config_file(nullConfigPath);
    ASSERT_TRUE (readNilResult.isResult());
    ASSERT_EQ (readNilResult.getResult(), tempo_config::ConfigNil());

    auto trueConfigPath = writeConfigFile("true.config", "true");
    auto readTrueResult = tempo_config::read_config_file(trueConfigPath);
    ASSERT_TRUE (readTrueResult.isResult());
    ASSERT_EQ (readTrueResult.getResult(), tempo_config::ConfigValue("true"));

    auto falseConfigPath = writeConfigFile("false.config", "false");
    auto readFalseResult = tempo_config::read_config_file(falseConfigPath);
    ASSERT_TRUE (readFalseResult.isResult());
    ASSERT_EQ (readFalseResult.getResult(), tempo_config::ConfigValue("false"));

    auto numberConfigPath = writeConfigFile("number.config", "42");
    auto readNumberResult = tempo_config::read_config_file(numberConfigPath);
    ASSERT_TRUE (readNumberResult.isResult());
    ASSERT_EQ (readNumberResult.getResult(), tempo_config::ConfigValue("42"));

    auto stringConfigPath = writeConfigFile("string.config", "\"hello, world!\"");
    auto readStringResult = tempo_config::read_config_file(stringConfigPath);
    ASSERT_TRUE (readStringResult.isResult());
    ASSERT_EQ (readStringResult.getResult(), tempo_config::ConfigValue("hello, world!"));
}

TEST_F(ConfigSerde, ReadConfigMapDirectory)
{
    auto map1ConfigPath = writeConfigFile("map1.config", "{\"map1\": null}");
    auto map2ConfigPath = writeConfigFile("map2.config", "{\"map2\": true}");
    auto map3ConfigPath = writeConfigFile("map3.config", "{\"map3\": false}");
    auto map4ConfigPath = writeConfigFile("map4.config", "{\"map4\": 42}");
    auto map5ConfigPath = writeConfigFile("map5.config", "{\"map5\": \"hello, world!\"}");

    auto readConfigMapResult = tempo_config::read_config_map_directory(rootDirectory);
    ASSERT_TRUE (readConfigMapResult.isResult());
    ASSERT_EQ (tempo_config::ConfigMap({
            {"map1", tempo_config::ConfigNil()},
            {"map2", tempo_config::ConfigValue("true")},
            {"map3", tempo_config::ConfigValue("false")},
            {"map4",  tempo_config::ConfigValue("42")},
            {"map5",  tempo_config::ConfigValue("hello, world!")},
        }),
        readConfigMapResult.getResult());
}

TEST_F(ConfigSerde, ReadConfigTreeDirectory)
{
    auto map1 = makeDirectory("map1");
    auto map2 = makeDirectory("map2");
    auto map3 = makeDirectory(std::filesystem::path("map2") / "map3");

    auto value1ConfigPath = writeConfigFile("value1.config", "null");
    auto value2ConfigPath = writeConfigFile(map1 / "value2.config", "true");
    auto value3ConfigPath = writeConfigFile(map1 / "value3.config", "false");
    auto value4ConfigPath = writeConfigFile(map2 / "value4.config", "42");
    auto value5ConfigPath = writeConfigFile(map3 / "value5.config", "\"hello, world!\"");

    auto readConfigTreeResult = tempo_config::read_config_tree_directory(rootDirectory);
    ASSERT_TRUE (readConfigTreeResult.isResult());
    ASSERT_EQ (tempo_config::ConfigMap({
            {"value1", tempo_config::ConfigNil()},
            {"map1", tempo_config::ConfigMap({
                {"value2", tempo_config::ConfigValue("true")},
                {"value3", tempo_config::ConfigValue("false")},
                })},
            {"map2", tempo_config::ConfigMap({
                {"value4", tempo_config::ConfigValue("42")},
                {"map3", tempo_config::ConfigMap({
                    {"value5", tempo_config::ConfigValue("hello, world!")},
                    })},
                })},
        }),
        readConfigTreeResult.getResult());
}
