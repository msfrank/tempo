
#include <gtest/gtest.h>

#include <boost/predef.h>

#include <tempo_utils/process_builder.h>
#include <tempo_utils/process_runner.h>
#include <tempo_utils/program_location.h>

#include "tempo_utils/file_utilities.h"

TEST(ProgramLocation, GetProgramPath)
{
    std::filesystem::path executablePath(PROGRAM_LOCATION_CHILD_EXECUTABLE);
    tempo_utils::ProcessBuilder builder(executablePath);
    builder.appendArg("-p");
    tempo_utils::ProcessRunner runner(builder.toInvoker());

    ASSERT_TRUE (runner.isValid());
    ASSERT_TRUE (runner.getStatus().isOk());
    ASSERT_EQ (executablePath.string(), runner.getChildOutput());
    ASSERT_EQ (0, runner.getExitStatus());
}

TEST(ProgramLocation, GetProgramDirectory)
{
    std::filesystem::path executablePath(PROGRAM_LOCATION_CHILD_EXECUTABLE);
    tempo_utils::ProcessBuilder builder(executablePath);
    builder.appendArg("-d");
    tempo_utils::ProcessRunner runner(builder.toInvoker());

    ASSERT_TRUE (runner.isValid());
    ASSERT_TRUE (runner.getStatus().isOk());
    ASSERT_EQ (executablePath.parent_path().string(), runner.getChildOutput());
    ASSERT_EQ (0, runner.getExitStatus());
}

TEST(ProgramLocation, GetProgramPathThroughSymlink)
{
    std::filesystem::path executablePath(PROGRAM_LOCATION_CHILD_EXECUTABLE);
    std::filesystem::path symlinkPath = std::filesystem::current_path() / tempo_utils::generate_name("symlink.XXXXXXXX");
    std::filesystem::create_symlink(executablePath, symlinkPath);
    tempo_utils::ProcessBuilder builder(symlinkPath);
    builder.appendArg("-p");
    tempo_utils::ProcessRunner runner(builder.toInvoker());

    ASSERT_TRUE (runner.isValid());
    ASSERT_TRUE (runner.getStatus().isOk());
    ASSERT_EQ (executablePath.string(), runner.getChildOutput());
    ASSERT_EQ (0, runner.getExitStatus());
}

TEST(ProgramLocation, GetProgramPathThroughMultipleSymlinks)
{
    std::filesystem::path executablePath(PROGRAM_LOCATION_CHILD_EXECUTABLE);
    std::vector<std::filesystem::path> symlinkPaths;

    for (int i = 0; i < tempo_utils::kMaxSymlinks; i++) {
        auto filename = tempo_utils::generate_name(absl::StrFormat("symlink%d.XXXXXXXX", i + 1));
        std::filesystem::path symlinkPath = std::filesystem::current_path() / filename;
        if (i == 0) {
            std::filesystem::create_symlink(executablePath, symlinkPath);
        } else {
            const auto &prevPath = symlinkPaths.at(i - 1);
            std::filesystem::create_symlink(prevPath, symlinkPath);
        }
        symlinkPaths.push_back(std::move(symlinkPath));
    }

    tempo_utils::ProcessBuilder builder(symlinkPaths.back());
    builder.appendArg("-p");
    tempo_utils::ProcessRunner runner(builder.toInvoker());

    ASSERT_TRUE (runner.isValid());
    ASSERT_TRUE (runner.getStatus().isOk());
    ASSERT_EQ (executablePath.string(), runner.getChildOutput());
    ASSERT_EQ (0, runner.getExitStatus());
}

#if BOOST_OS_MACOS
TEST(ProgramLocation, GetProgramPathFailsWhenExceedingMaxSymlinks)
{
    std::filesystem::path executablePath(PROGRAM_LOCATION_CHILD_EXECUTABLE);
    std::vector<std::filesystem::path> symlinkPaths;

    for (int i = 0; i < tempo_utils::kMaxSymlinks + 1; i++) {
        auto filename = tempo_utils::generate_name(absl::StrFormat("symlink%d.XXXXXXXX", i + 1));
        std::filesystem::path symlinkPath = std::filesystem::current_path() / filename;
        if (i == 0) {
            std::filesystem::create_symlink(executablePath, symlinkPath);
        } else {
            const auto &prevPath = symlinkPaths.at(i - 1);
            std::filesystem::create_symlink(prevPath, symlinkPath);
        }
        symlinkPaths.push_back(std::move(symlinkPath));
    }

    tempo_utils::ProcessBuilder builder(symlinkPaths.back());
    builder.appendArg("-p");
    tempo_utils::ProcessRunner runner(builder.toInvoker());

    ASSERT_TRUE (runner.isValid());
    ASSERT_TRUE (runner.getStatus().isOk());
    TU_CONSOLE_OUT << "output of " PROGRAM_LOCATION_CHILD_EXECUTABLE << " is: " << runner.getChildOutput();
    ASSERT_EQ (1, runner.getExitStatus());
}
#endif