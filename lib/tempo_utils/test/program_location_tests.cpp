
#include <gtest/gtest.h>

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

TEST(ProgramLocation, GetSymlinkedProgramPath)
{
    std::filesystem::path executablePath(PROGRAM_LOCATION_CHILD_EXECUTABLE);
    std::filesystem::path symlinkPath = std::filesystem::current_path() / tempo_utils::generate_name("symlink.XXXXXXXX");
    std::filesystem::create_symlink(executablePath, symlinkPath);
    tempo_utils::ProcessBuilder builder(symlinkPath);
    builder.appendArg("-p");
    tempo_utils::ProcessRunner runner(builder.toInvoker());

    ASSERT_TRUE (runner.isValid());
    ASSERT_TRUE (runner.getStatus().isOk());
    ASSERT_EQ (symlinkPath.string(), runner.getChildOutput());
    ASSERT_EQ (0, runner.getExitStatus());
}
