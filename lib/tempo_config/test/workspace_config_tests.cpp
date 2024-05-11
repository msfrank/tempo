#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <tempo_config/config_types.h>
#include <tempo_config/workspace_config.h>
#include <tempo_utils/directory_maker.h>
#include <tempo_utils/file_writer.h>
#include <tempo_utils/tempdir_maker.h>

class WorkspaceConfig : public ::testing::Test {
protected:

    std::filesystem::path rootDirectory;
    std::filesystem::path workspaceRootDirectory;
    std::filesystem::path workspaceVendorConfigDirectory;
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

        tempo_utils::DirectoryMaker workspaceMaker(rootDirectory, "workspace");
        ASSERT_TRUE (workspaceMaker.isValid());
        workspaceRootDirectory = workspaceMaker.getAbsolutePath();


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

TEST_F(WorkspaceConfig, TestParseWorkspaceConfigOnly)
{
    tempo_utils::FileWriter workspaceConfigWriter(workspaceRootDirectory / "workspace.config", R"(
        { "one": 1 }
    )", tempo_utils::FileWriterMode::CREATE_OR_OVERWRITE);

    tempo_config::WorkspaceConfigOptions options;
    auto loadWorkspaceConfigResult = tempo_config::WorkspaceConfig::load(
        workspaceConfigWriter.getAbsolutePath(), options);
    ASSERT_TRUE (loadWorkspaceConfigResult.isResult());
    auto workspace = loadWorkspaceConfigResult.getResult();
    auto config = workspace->getWorkspaceConfig();
    ASSERT_EQ (tempo_config::ConfigMap({{"one", tempo_config::ConfigValue("1")}}), config);
}

TEST_F(WorkspaceConfig, TestParseWorkspaceUserAndDistConfig)
{
    tempo_utils::FileWriter workspaceConfigWriter(workspaceRootDirectory / "workspace.config", R"(
        { "zero": 0 }
    )", tempo_utils::FileWriterMode::CREATE_OR_OVERWRITE);

    tempo_utils::FileWriter userConfigWriter(userConfigDirectory / "one.config", "1",
        tempo_utils::FileWriterMode::CREATE_OR_OVERWRITE);
    tempo_utils::FileWriter distConfigWriter(distConfigDirectory / "two.config", "2",
        tempo_utils::FileWriterMode::CREATE_OR_OVERWRITE);
    tempo_utils::FileWriter userVendorConfigWriter(userVendorConfigDirectory / "three.config", "3",
        tempo_utils::FileWriterMode::CREATE_OR_OVERWRITE);
    tempo_utils::FileWriter distVendorConfigWriter(distVendorConfigDirectory / "four.config", "4",
        tempo_utils::FileWriterMode::CREATE_OR_OVERWRITE);

    tempo_config::WorkspaceConfigOptions options;
    options.userConfigDirectoryPath = userConfigDirectory;
    options.userVendorConfigDirectoryPath = userVendorConfigDirectory;
    options.distConfigDirectoryPath = distConfigDirectory;
    options.distVendorConfigDirectoryPath = distVendorConfigDirectory;

    auto loadWorkspaceConfigResult = tempo_config::WorkspaceConfig::load(
        workspaceConfigWriter.getAbsolutePath(), options);
    ASSERT_TRUE (loadWorkspaceConfigResult.isResult());

    auto workspace = loadWorkspaceConfigResult.getResult();

    ASSERT_EQ (tempo_config::ConfigMap({
            {"zero", tempo_config::ConfigValue("0")},
            {"one", tempo_config::ConfigValue("1")},
            {"two", tempo_config::ConfigValue("2")},
        }),
        workspace->getWorkspaceConfig());
    ASSERT_EQ (tempo_config::ConfigMap({
            {"three", tempo_config::ConfigValue("3")},
            {"four", tempo_config::ConfigValue("4")},
        }),
        workspace->getVendorConfig());
}
