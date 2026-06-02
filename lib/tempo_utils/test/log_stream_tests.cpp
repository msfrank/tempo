#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_utils/tempo_utils.h>

class LogStream : public ::testing::Test {};

TEST_F (LogStream, StringLiteral)
{
    std::string output;
    tempo_utils::LogToString(&output) << "hello, world!";
    ASSERT_EQ ("hello, world!", output);
}

TEST_F (LogStream, CharPointer)
{
    std::string output;
    const char *str = "hello, world!";
    tempo_utils::LogToString(&output) << str;
    ASSERT_EQ ("hello, world!", output);
}

TEST_F (LogStream, StdString)
{
    std::string output;
    std::string message = "hello, world!";
    tempo_utils::LogToString(&output) << message;
    ASSERT_EQ (message, output);
}

TEST_F (LogStream, StdStringView)
{
    std::string output;
    std::string_view message = "hello, world!";
    tempo_utils::LogToString(&output) << message;
    ASSERT_EQ (message, output);
}

TEST_F (LogStream, IntLiteral)
{
    std::string output;
    tempo_utils::LogToString(&output) << 42;
    ASSERT_EQ ("42", output);
}

TEST_F (LogStream, Int8Literal)
{
    std::string output;
    tu_int8 value = -8;
    tempo_utils::LogToString(&output) << value;
    ASSERT_EQ ("-8", output);
}

TEST_F (LogStream, Int16Literal)
{
    std::string output;
    tu_int16 value = -16;
    tempo_utils::LogToString(&output) << value;
    ASSERT_EQ ("-16", output);
}

TEST_F (LogStream, Int32Literal)
{
    std::string output;
    tu_int32 value = -32;
    tempo_utils::LogToString(&output) << value;
    ASSERT_EQ ("-32", output);
}

TEST_F (LogStream, Int64Literal)
{
    std::string output;
    tu_int64 value = -64;
    tempo_utils::LogToString(&output) << value;
    ASSERT_EQ ("-64", output);
}

TEST_F (LogStream, UInt8Literal)
{
    std::string output;
    tu_uint8 value = 8;
    tempo_utils::LogToString(&output) << value;
    ASSERT_EQ ("8", output);
}

TEST_F (LogStream, UInt16Literal)
{
    std::string output;
    tu_uint16 value = 16;
    tempo_utils::LogToString(&output) << value;
    ASSERT_EQ ("16", output);
}

TEST_F (LogStream, UInt32Literal)
{
    std::string output;
    tu_uint32 value = 32;
    tempo_utils::LogToString(&output) << value;
    ASSERT_EQ ("32", output);
}

TEST_F (LogStream, UInt64Literal)
{
    std::string output;
    tu_uint64 value = 64;
    tempo_utils::LogToString(&output) << value;
    ASSERT_EQ ("64", output);
}

TEST_F (LogStream, DoubleLiteral)
{
    std::string output;
    tempo_utils::LogToString(&output) << 3.14159;
    ASSERT_EQ ("3.14159", output);
}

TEST_F (LogStream, FloatLiteral)
{
    std::string output;
    tempo_utils::LogToString(&output) << 3.14159f;
    ASSERT_EQ ("3.14159", output);
}

TEST_F (LogStream, BoolLiteral)
{
    std::string output;
    tempo_utils::LogToString(&output) << true;
    ASSERT_EQ ("true", output);

    output.clear();
    tempo_utils::LogToString(&output) << false;
    ASSERT_EQ ("false", output);
}
