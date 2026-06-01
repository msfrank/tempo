#include <absl/strings/str_split.h>
#include <gtest/gtest.h>

#include <tempo_utils/tempo_utils.h>

TEST(Logging, TestLoggingStreamTypes)
{
    TU_LOG_INFO << "char * ... " << "hello!";
    TU_LOG_INFO << "std::string ... " << std::string("hello!");
    TU_LOG_INFO << "int ... " << 42;
    TU_LOG_INFO << "uint64_t ... " << static_cast<uint64_t>(10000000000);
    TU_LOG_INFO << "int64_t ... " << static_cast<int64_t>(-10000000000);
    TU_LOG_INFO << "double ... " << static_cast<double>(3.14159265359);
    TU_LOG_INFO << "bool ... " << true;
}

TEST(Logging, TestLoggingPipeline)
{
    TU_LOG_INFO << "hello, " << "info!";
}

TEST(Logging, TestConditionalLogging)
{
    int x = 42;
    TU_LOG_INFO_IF(x > 0) << "hello, " << "info!";
    TU_LOG_INFO_IF(x > 100) << "should not print anything";
}

TEST(Logging, TestInitialBuffering)
{

    tempo_utils::ProcessBuilder builder(LOGGING_CHILD_EXECUTABLE);
    builder.appendArg("1");
    builder.appendArg("2");
    builder.appendArg("3");
    builder.appendArg("--start");

    tempo_utils::ProcessRunner runner(builder.toInvoker());
    ASSERT_TRUE (runner.isValid());
    ASSERT_TRUE (runner.getStatus().isOk());
    ASSERT_EQ (0, runner.getExitStatus());
    ASSERT_EQ ("", runner.getChildError());

    std::vector<std::string> lines = absl::StrSplit(runner.getChildOutput(), '\n', absl::SkipEmpty());
    ASSERT_EQ (3, lines.size());
    ASSERT_EQ ("1", lines.at(0));
    ASSERT_EQ ("2", lines.at(1));
    ASSERT_EQ ("3", lines.at(2));
}

TEST(Logging, TestCleanupFiltering)
{

    tempo_utils::ProcessBuilder builder(LOGGING_CHILD_EXECUTABLE);
    builder.appendArg("1");
    builder.appendArg("--start");
    builder.appendArg("2");
    builder.appendArg("--finish");
    builder.appendArg("3");

    tempo_utils::ProcessRunner runner(builder.toInvoker());
    ASSERT_TRUE (runner.isValid());
    ASSERT_TRUE (runner.getStatus().isOk());
    ASSERT_EQ (0, runner.getExitStatus());
    ASSERT_EQ ("", runner.getChildError());

    std::vector<std::string> lines = absl::StrSplit(runner.getChildOutput(), '\n', absl::SkipEmpty());
    ASSERT_EQ (2, lines.size());
    ASSERT_EQ ("1", lines.at(0));
    ASSERT_EQ ("2", lines.at(1));
}

TEST(Logging, TestRestartLogging)
{

    tempo_utils::ProcessBuilder builder(LOGGING_CHILD_EXECUTABLE);
    builder.appendArg("1");
    builder.appendArg("--start");
    builder.appendArg("2");
    builder.appendArg("--stop");
    builder.appendArg("3");
    builder.appendArg("--start");
    builder.appendArg("4");
    builder.appendArg("--finish");
    builder.appendArg("5");

    tempo_utils::ProcessRunner runner(builder.toInvoker());
    ASSERT_TRUE (runner.isValid());
    ASSERT_TRUE (runner.getStatus().isOk());
    ASSERT_EQ (0, runner.getExitStatus());
    ASSERT_EQ ("", runner.getChildError());

    std::vector<std::string> lines = absl::StrSplit(runner.getChildOutput(), '\n', absl::SkipEmpty());
    ASSERT_EQ (4, lines.size());
    ASSERT_EQ ("1", lines.at(0));
    ASSERT_EQ ("2", lines.at(1));
    ASSERT_EQ ("3", lines.at(2));
    ASSERT_EQ ("4", lines.at(3));
}
