
#include <gtest/gtest.h>

#include <tempo_utils/process_runner.h>
#include <tempo_utils/process_builder.h>

TEST(ProcessRunner, CaptureProcessChildOutput)
{
    tempo_utils::ProcessBuilder builder(PROCESS_RUNNER_CHILD_EXECUTABLE);
    builder.appendArg("-o", "Hello, world!");
    tempo_utils::ProcessRunner runner(builder.toInvoker());
    ASSERT_TRUE (runner.isValid());
    ASSERT_TRUE (runner.getStatus().isOk());
    ASSERT_EQ (0, runner.getExitStatus());
    ASSERT_EQ ("Hello, world!", runner.getChildOutput());
    ASSERT_EQ ("", runner.getChildError());
}

TEST(ProcessRunner, CaptureProcessChildError)
{
    tempo_utils::ProcessBuilder builder(PROCESS_RUNNER_CHILD_EXECUTABLE);
    builder.appendArg("-e", "Hello, error!");
    tempo_utils::ProcessRunner runner(builder.toInvoker());
    ASSERT_TRUE (runner.isValid());
    ASSERT_TRUE (runner.getStatus().isOk());
    ASSERT_EQ (0, runner.getExitStatus());
    ASSERT_EQ ("Hello, error!", runner.getChildError());
    ASSERT_EQ ("", runner.getChildOutput());
}

TEST(ProcessRunner, CaptureProcessExitStatus)
{
    tempo_utils::ProcessBuilder builder(PROCESS_RUNNER_CHILD_EXECUTABLE);
    builder.appendArg("-x", "42");
    tempo_utils::ProcessRunner runner(builder.toInvoker());
    ASSERT_TRUE (runner.isValid());
    ASSERT_TRUE (runner.getStatus().isOk());
    ASSERT_EQ (42, runner.getExitStatus());
    ASSERT_EQ ("", runner.getChildOutput());
    ASSERT_EQ ("", runner.getChildError());
}

TEST(ProcessRunner, FailsWhenExecutableIsMissing)
{
    tempo_utils::ProcessBuilder builder("does-not-exist");
    tempo_utils::ProcessRunner runner(builder.toInvoker());
    ASSERT_FALSE (runner.isValid());
    auto status = runner.getStatus();
}
