#include <gtest/gtest.h>

#include <tempo_command/command.h>
#include <tempo_command/logging_environment.h>
#include <tempo_config/base_conversions.h>
#include <tempo_utils/logging.h>

class TestEnvironment : public ::testing::Environment {
public:
    explicit TestEnvironment(std::filesystem::path loggingConfigFile, const tempo_config::ConfigMap &loggingOverrides)
        : m_loggingConfigFile(std::move(loggingConfigFile)),
          m_loggingOverrides(loggingOverrides)
        {}

    void SetUp() override {
        tempo_command::init_logging_from_environment(m_loggingConfigFile, m_loggingOverrides);
    }
    void TearDown() override {
        tempo_utils::cleanup_logging();
    }

private:
    std::filesystem::path m_loggingConfigFile;
    tempo_config::ConfigMap m_loggingOverrides;
};

int main(int argc, char **argv)
{
    auto *commandName = argc > 0? argv[0] : "testsuite";

    ::testing::InitGoogleTest(&argc, argv);

    tempo_command::Command command(commandName);
    command.addHelpOption("help", {"-h", "--help"},
        absl::StrFormat("Run tests in %s", commandName));

    command.addOption("loggingConfigFile", {"--logging-config-file"},
        tempo_command::MappingType::ZERO_OR_ONE_INSTANCE,
        "load logging config from FILE", "FILE");
    command.addKeyValueOption("loggingOverrides", {"-o", "--logging-option"},
        tempo_command::MappingType::INSTANCE_HASH,
        "override logging option value", "OPTION");

    TU_RAISE_IF_NOT_OK (command.parse(argc - 1, (const char **) &argv[1]));

    tempo_config::PathParser loggingConfigFileParser(std::filesystem::path{});
    std::filesystem::path loggingConfigFile;
    TU_RAISE_IF_NOT_OK (command.convert(loggingConfigFile, loggingConfigFileParser, "loggingConfigFile"));

    auto loggingOverrides = command.getConfig("loggingOverrides");

    ::testing::AddGlobalTestEnvironment(new TestEnvironment(
        loggingConfigFile, loggingOverrides.toMap()));

    return RUN_ALL_TESTS();
}
