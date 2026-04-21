
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include <tempo_utils/file_appender.h>

#include "../../tempo_test/include/tempo_test/tempo_test.h"
#include "tempo_utils/bytes_iterator.h"
#include "tempo_utils/file_reader.h"
#include "tempo_utils/file_utilities.h"

class FileAppender : public ::testing::Test {
protected:
    std::filesystem::path filePath;
    void SetUp() override {
        filePath = std::filesystem::current_path() / tempo_utils::generate_name("appender.XXXXXXXX");
    }
    void TearDown() override {
        if (std::filesystem::exists(filePath)) {
            std::filesystem::remove(filePath);
        }
    }
};

TEST_F(FileAppender, AppendU8AndReadBack)
{
    tempo_utils::FileAppender appender(filePath);
    ASSERT_THAT (appender.appendU8(0x08), tempo_test::IsOk());
    ASSERT_THAT (appender.finish(), tempo_test::IsOk());

    tempo_utils::FileReader reader(filePath);
    ASSERT_THAT (reader.getStatus(), tempo_test::IsOk());

    tempo_utils::BytesIterator it(reader.getSpan());
    tu_uint8 u8;
    ASSERT_TRUE (it.nextU8(u8));
    ASSERT_EQ (0x08, u8);
    ASSERT_FALSE (it.isValid());
}

TEST_F(FileAppender, AppendU16AndReadBack)
{
    tempo_utils::FileAppender appender(filePath);
    ASSERT_THAT (appender.appendU16(0x1616), tempo_test::IsOk());
    ASSERT_THAT (appender.finish(), tempo_test::IsOk());

    tempo_utils::FileReader reader(filePath);
    ASSERT_THAT (reader.getStatus(), tempo_test::IsOk());

    tempo_utils::BytesIterator it(reader.getSpan());
    tu_uint16 u16;
    ASSERT_TRUE (it.nextU16(u16));
    ASSERT_EQ (0x1616, u16);
    ASSERT_FALSE (it.isValid());
}

TEST_F(FileAppender, AppendU32AndReadBack)
{
    tempo_utils::FileAppender appender(filePath);
    ASSERT_THAT (appender.appendU32(0x32323232), tempo_test::IsOk());
    ASSERT_THAT (appender.finish(), tempo_test::IsOk());

    tempo_utils::FileReader reader(filePath);
    ASSERT_THAT (reader.getStatus(), tempo_test::IsOk());

    tempo_utils::BytesIterator it(reader.getSpan());
    tu_uint32 u32;
    ASSERT_TRUE (it.nextU32(u32));
    ASSERT_EQ (0x32323232, u32);
    ASSERT_FALSE (it.isValid());
}

TEST_F(FileAppender, AppendU64AndReadBack)
{
    tempo_utils::FileAppender appender(filePath);
    ASSERT_THAT (appender.appendU64(0x6464646464646464), tempo_test::IsOk());
    ASSERT_THAT (appender.finish(), tempo_test::IsOk());

    tempo_utils::FileReader reader(filePath);
    ASSERT_THAT (reader.getStatus(), tempo_test::IsOk());

    tempo_utils::BytesIterator it(reader.getSpan());
    tu_uint64 u64;
    ASSERT_TRUE (it.nextU64(u64));
    ASSERT_EQ (0x6464646464646464, u64);
    ASSERT_FALSE (it.isValid());
}

TEST_F(FileAppender, AppendF32AndReadBack)
{
    tempo_utils::FileAppender appender(filePath);
    ASSERT_THAT (appender.appendF32(123456.789), tempo_test::IsOk());
    ASSERT_THAT (appender.finish(), tempo_test::IsOk());

    tempo_utils::FileReader reader(filePath);
    ASSERT_THAT (reader.getStatus(), tempo_test::IsOk());

    tempo_utils::BytesIterator it(reader.getSpan());
    float f32;
    ASSERT_TRUE (it.nextF32(f32));
    ASSERT_FLOAT_EQ (123456.789, f32);
    ASSERT_FALSE (it.isValid());
}

TEST_F(FileAppender, AppendF64AndReadBack)
{
    tempo_utils::FileAppender appender(filePath);
    ASSERT_THAT (appender.appendF64(12345645678912345.4321), tempo_test::IsOk());
    ASSERT_THAT (appender.finish(), tempo_test::IsOk());

    tempo_utils::FileReader reader(filePath);
    ASSERT_THAT (reader.getStatus(), tempo_test::IsOk());

    tempo_utils::BytesIterator it(reader.getSpan());
    double f64;
    ASSERT_TRUE (it.nextF64(f64));
    ASSERT_DOUBLE_EQ (12345645678912345.4321, f64);
    ASSERT_FALSE (it.isValid());
}
