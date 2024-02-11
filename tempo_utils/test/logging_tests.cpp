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