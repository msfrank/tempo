
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <tempo_config/config_types.h>
#include <tempo_config/program_config.h>
#include <tempo_utils/directory_maker.h>
#include <tempo_utils/file_writer.h>
#include <tempo_utils/tempdir_maker.h>

class ProgramConfig : public ::testing::Test {
protected:

    std::filesystem::path rootDirectory;
    std::filesystem::path userHomeDirectory;
    std::filesystem::path userConfigDirectory;
    std::filesystem::path userVendorConfigDirectory;
    std::filesystem::path distRootDirectory;
    std::filesystem::path distConfigDirectory;
    std::filesystem::path distVendorConfigDirectory;

    void SetUp() override {
        tempo_utils::TempdirMaker tempdirMaker(std::filesystem::current_path(), "root.XXXXXXXX");
        ASSERT_TRUE (tempdirMaker.isValid());
        rootDirectory = tempdirMaker.getTempdir();

        tempo_utils::DirectoryMaker userHomeMaker(rootDirectory, std::filesystem::path("home"));
        ASSERT_TRUE (userHomeMaker.isValid());
        userHomeDirectory = userHomeMaker.getAbsolutePath();

        tempo_utils::DirectoryMaker userConfigMaker(userHomeDirectory, tempo_config::kDefaultConfigDirectoryName);
        ASSERT_TRUE (userConfigMaker.isValid());
        userConfigDirectory = userConfigMaker.getAbsolutePath();

        tempo_utils::DirectoryMaker userVendorConfigMaker(userHomeDirectory, tempo_config::kDefaultVendorConfigDirectoryName);
        ASSERT_TRUE (userVendorConfigMaker.isValid());
        userVendorConfigDirectory = userVendorConfigMaker.getAbsolutePath();

        tempo_utils::DirectoryMaker distMaker(rootDirectory, std::filesystem::path("dist"));
        ASSERT_TRUE (distMaker.isValid());
        distRootDirectory = distMaker.getAbsolutePath();

        tempo_utils::DirectoryMaker distConfigMaker(distRootDirectory, tempo_config::kDefaultConfigDirectoryName);
        ASSERT_TRUE (distConfigMaker.isValid());
        distConfigDirectory = distConfigMaker.getAbsolutePath();

        tempo_utils::DirectoryMaker distVendorConfigMaker(distRootDirectory, tempo_config::kDefaultVendorConfigDirectoryName);
        ASSERT_TRUE (distVendorConfigMaker.isValid());
        distVendorConfigDirectory = distVendorConfigMaker.getAbsolutePath();
    }

    void TearDown() override {
        std::filesystem::remove_all(rootDirectory);
    }
};

TEST_F(ProgramConfig, TestParseUserAndDistConfig)
{
    tempo_utils::FileWriter userConfigWriter(userConfigDirectory / "one.config", "1",
        tempo_utils::FileWriterMode::CREATE_OR_OVERWRITE);
    tempo_utils::FileWriter distConfigWriter(distConfigDirectory / "two.config", "2",
        tempo_utils::FileWriterMode::CREATE_OR_OVERWRITE);
    tempo_utils::FileWriter userVendorConfigWriter(userVendorConfigDirectory / "three.config", "3",
        tempo_utils::FileWriterMode::CREATE_OR_OVERWRITE);
    tempo_utils::FileWriter distVendorConfigWriter(distVendorConfigDirectory / "four.config", "4",
        tempo_utils::FileWriterMode::CREATE_OR_OVERWRITE);

    tempo_config::ProgramConfigOptions options;
    options.userConfigDirectoryPath = userConfigDirectory;
    options.userVendorConfigDirectoryPath = userVendorConfigDirectory;
    options.distConfigDirectoryPath = distConfigDirectory;
    options.distVendorConfigDirectoryPath = distVendorConfigDirectory;

    auto loadProgramConfigResult = tempo_config::ProgramConfig::load(options);
    ASSERT_TRUE (loadProgramConfigResult.isResult());

    auto environment = loadProgramConfigResult.getResult();
    ASSERT_EQ (tempo_config::ConfigMap({
            {"one", tempo_config::ConfigValue("1")},
            {"two", tempo_config::ConfigValue("2")},
        }),
        environment->getProgramConfig());
    ASSERT_EQ (tempo_config::ConfigMap({
            {"three", tempo_config::ConfigValue("3")},
            {"four", tempo_config::ConfigValue("4")},
        }),
        environment->getVendorConfig());
}
