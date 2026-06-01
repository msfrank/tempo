#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_utils/log_helpers.h>

class LogHelpers : public ::testing::Test {};

TEST_F (LogHelpers, PrintBinaryForU8)
{
    std::string output;
    tempo_utils::LogToString(&output) << tempo_utils::Bin<tu_uint8>(67);
    ASSERT_EQ ("0b01000011", output);
}

TEST_F (LogHelpers, PrintBinaryForU16)
{
    std::string output;
    tempo_utils::LogToString(&output) << tempo_utils::Bin<tu_uint16>(6414);
    ASSERT_EQ ("0b0001100100001110", output);
}

TEST_F (LogHelpers, PrintBinaryForU32)
{
    std::string output;
    tempo_utils::LogToString(&output) << tempo_utils::Bin<tu_uint32>(1837456927);
    ASSERT_EQ ("0b01101101100001010101111000011111", output);
}

TEST_F (LogHelpers, PrintBinaryForU64)
{
    std::string output;
    tempo_utils::LogToString(&output) << tempo_utils::Bin<tu_uint64>(29943881837456927);
    ASSERT_EQ ("0b0000000001101010011000011100110101001000000111100010111000011111", output);
}

TEST_F (LogHelpers, PrintBinaryOmittingPrefix)
{
    std::string output;
    tempo_utils::LogToString(&output) << tempo_utils::Bin<tu_uint8>(67, false);
    ASSERT_EQ ("01000011", output);
}

TEST_F (LogHelpers, PrintBinarySpecifyingPrefix)
{
    std::string output;
    tempo_utils::LogToString(&output) << tempo_utils::Bin<tu_uint8>(67, true, "BINARY:");
    ASSERT_EQ ("BINARY:01000011", output);
}

TEST_F (LogHelpers, PrintHexForU8)
{
    std::string output;
    tempo_utils::LogToString(&output) << tempo_utils::Hex<tu_uint8>(185);
    ASSERT_EQ ("0xb9", output);
}
