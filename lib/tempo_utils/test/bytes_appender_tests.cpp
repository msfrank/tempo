
#include <gtest/gtest.h>

#include <tempo_utils/bytes_appender.h>

class BytesAppender : public ::testing::Test {};

TEST_F(BytesAppender, TestReadAndWriteU8)
{
    tempo_utils::BytesAppender appender;
    appender.appendU8(0x42);
    auto bytes = appender.finish();
    ASSERT_TRUE (bytes != nullptr);
    ASSERT_EQ (1, bytes->getSize());
    auto *data = bytes->getData();
    ASSERT_EQ (0x42, (tu_uint8) data[0]);
}

TEST_F(BytesAppender, TestReadAndWriteU16)
{
    tempo_utils::BytesAppender appender;
    appender.appendU16(0x55AA);
    auto bytes = appender.finish();
    ASSERT_TRUE (bytes != nullptr);
    ASSERT_EQ (2, bytes->getSize());
    auto *data = bytes->getData();
    ASSERT_EQ (0x55, (tu_uint8) data[0]);
    ASSERT_EQ (0xAA, (tu_uint8) data[1]);
}

TEST_F(BytesAppender, TestReadAndWriteU16LE)
{
    tempo_utils::BytesAppender appender;
    appender.appendU16LE(0x55AA);
    auto bytes = appender.finish();
    ASSERT_TRUE (bytes != nullptr);
    ASSERT_EQ (2, bytes->getSize());
    auto *data = bytes->getData();
    ASSERT_EQ (0xAA, (tu_uint8) data[0]);
    ASSERT_EQ (0x55, (tu_uint8) data[1]);
}

TEST_F(BytesAppender, TestReadAndWriteU32)
{
    tempo_utils::BytesAppender appender;
    appender.appendU32(0x55AA9922);
    auto bytes = appender.finish();
    ASSERT_TRUE (bytes != nullptr);
    ASSERT_EQ (4, bytes->getSize());
    auto *data = bytes->getData();
    ASSERT_EQ (0x55, (tu_uint8) data[0]);
    ASSERT_EQ (0xAA, (tu_uint8) data[1]);
    ASSERT_EQ (0x99, (tu_uint8) data[2]);
    ASSERT_EQ (0x22, (tu_uint8) data[3]);
}

TEST_F(BytesAppender, TestReadAndWriteU32LE)
{
    tempo_utils::BytesAppender appender;
    appender.appendU32LE(0x55AA9922);
    auto bytes = appender.finish();
    ASSERT_TRUE (bytes != nullptr);
    ASSERT_EQ (4, bytes->getSize());
    auto *data = bytes->getData();
    ASSERT_EQ (0x22, (tu_uint8) data[0]);
    ASSERT_EQ (0x99, (tu_uint8) data[1]);
    ASSERT_EQ (0xAA, (tu_uint8) data[2]);
    ASSERT_EQ (0x55, (tu_uint8) data[3]);
}

TEST_F(BytesAppender, TestReadAndWriteU64)
{
    tempo_utils::BytesAppender appender;
    appender.appendU64(0x55AA992233771122);
    auto bytes = appender.finish();
    ASSERT_TRUE (bytes != nullptr);
    ASSERT_EQ (8, bytes->getSize());
    auto *data = bytes->getData();
    ASSERT_EQ (0x55, (tu_uint8) data[0]);
    ASSERT_EQ (0xAA, (tu_uint8) data[1]);
    ASSERT_EQ (0x99, (tu_uint8) data[2]);
    ASSERT_EQ (0x22, (tu_uint8) data[3]);
    ASSERT_EQ (0x33, (tu_uint8) data[4]);
    ASSERT_EQ (0x77, (tu_uint8) data[5]);
    ASSERT_EQ (0x11, (tu_uint8) data[6]);
    ASSERT_EQ (0x22, (tu_uint8) data[7]);
}

TEST_F(BytesAppender, TestReadAndWriteU64LE)
{
    tempo_utils::BytesAppender appender;
    appender.appendU64LE(0x55AA992233771122);
    auto bytes = appender.finish();
    ASSERT_TRUE (bytes != nullptr);
    ASSERT_EQ (8, bytes->getSize());
    auto *data = bytes->getData();
    ASSERT_EQ (0x22, (tu_uint8) data[0]);
    ASSERT_EQ (0x11, (tu_uint8) data[1]);
    ASSERT_EQ (0x77, (tu_uint8) data[2]);
    ASSERT_EQ (0x33, (tu_uint8) data[3]);
    ASSERT_EQ (0x22, (tu_uint8) data[4]);
    ASSERT_EQ (0x99, (tu_uint8) data[5]);
    ASSERT_EQ (0xAA, (tu_uint8) data[6]);
    ASSERT_EQ (0x55, (tu_uint8) data[7]);
}

TEST_F(BytesAppender, TestReadAndWriteF32)
{
    tempo_utils::BytesAppender appender;
    appender.appendF32(0.0123);
    auto bytes = appender.finish();
    ASSERT_TRUE (bytes != nullptr);
    ASSERT_EQ (4, bytes->getSize());
    auto *data = bytes->getData();
    ASSERT_EQ (0x3C, (tu_uint8) data[0]);
    ASSERT_EQ (0x49, (tu_uint8) data[1]);
    ASSERT_EQ (0x85, (tu_uint8) data[2]);
    ASSERT_EQ (0xF0, (tu_uint8) data[3]);
}

TEST_F(BytesAppender, TestReadAndWriteF32LE)
{
    tempo_utils::BytesAppender appender;
    appender.appendF32LE(0.0123);
    auto bytes = appender.finish();
    ASSERT_TRUE (bytes != nullptr);
    ASSERT_EQ (4, bytes->getSize());
    auto *data = bytes->getData();
    ASSERT_EQ (0xF0, (tu_uint8) data[0]);
    ASSERT_EQ (0x85, (tu_uint8) data[1]);
    ASSERT_EQ (0x49, (tu_uint8) data[2]);
    ASSERT_EQ (0x3C, (tu_uint8) data[3]);
}

TEST_F(BytesAppender, TestReadAndWriteF64)
{
    tempo_utils::BytesAppender appender;
    appender.appendF64(112233444455556666.789);
    auto bytes = appender.finish();
    ASSERT_TRUE (bytes != nullptr);
    ASSERT_EQ (8, bytes->getSize());
    auto *data = bytes->getData();
    // 4378 EBBB 91CB 1264
    ASSERT_EQ (0x43, (tu_uint8) data[0]);
    ASSERT_EQ (0x78, (tu_uint8) data[1]);
    ASSERT_EQ (0xEB, (tu_uint8) data[2]);
    ASSERT_EQ (0xBB, (tu_uint8) data[3]);
    ASSERT_EQ (0x91, (tu_uint8) data[4]);
    ASSERT_EQ (0xCB, (tu_uint8) data[5]);
    ASSERT_EQ (0x12, (tu_uint8) data[6]);
    ASSERT_EQ (0x64, (tu_uint8) data[7]);
}

TEST_F(BytesAppender, TestReadAndWriteF64LE)
{
    tempo_utils::BytesAppender appender;
    appender.appendF64LE(112233444455556666.789);
    auto bytes = appender.finish();
    ASSERT_TRUE (bytes != nullptr);
    ASSERT_EQ (8, bytes->getSize());
    auto *data = bytes->getData();
    ASSERT_EQ (0x64, (tu_uint8) data[0]);
    ASSERT_EQ (0x12, (tu_uint8) data[1]);
    ASSERT_EQ (0xCB, (tu_uint8) data[2]);
    ASSERT_EQ (0x91, (tu_uint8) data[3]);
    ASSERT_EQ (0xBB, (tu_uint8) data[4]);
    ASSERT_EQ (0xEB, (tu_uint8) data[5]);
    ASSERT_EQ (0x78, (tu_uint8) data[6]);
    ASSERT_EQ (0x43, (tu_uint8) data[7]);
}

TEST_F(BytesAppender, TestReadAndWriteStdSpan)
{
    tempo_utils::BytesAppender appender;
    std::vector<tu_uint8> src = {0x01, 0x02, 0x03};
    appender.appendBytes(src);
    auto bytes = appender.finish();
    ASSERT_TRUE (bytes != nullptr);
    ASSERT_EQ (3, bytes->getSize());
    auto *data = bytes->getData();
    ASSERT_EQ (0x01, (tu_uint8) data[0]);
    ASSERT_EQ (0x02, (tu_uint8) data[1]);
    ASSERT_EQ (0x03, (tu_uint8) data[2]);
}

TEST_F(BytesAppender, TestReadAndWriteStdString)
{
    tempo_utils::BytesAppender appender;
    std::string src = {0x01, 0x02, 0x03};
    appender.appendBytes(src);
    auto bytes = appender.finish();
    ASSERT_TRUE (bytes != nullptr);
    ASSERT_EQ (3, bytes->getSize());
    auto *data = bytes->getData();
    ASSERT_EQ (0x01, (tu_uint8) data[0]);
    ASSERT_EQ (0x02, (tu_uint8) data[1]);
    ASSERT_EQ (0x03, (tu_uint8) data[2]);
}

TEST_F(BytesAppender, TestReadAndWriteStdStringView)
{
    tempo_utils::BytesAppender appender;
    char src[3] = {0x01, 0x02, 0x03};
    std::string_view sv(src, 3);
    appender.appendBytes(sv);
    auto bytes = appender.finish();
    ASSERT_TRUE (bytes != nullptr);
    ASSERT_EQ (3, bytes->getSize());
    auto *data = bytes->getData();
    ASSERT_EQ (0x01, (tu_uint8) data[0]);
    ASSERT_EQ (0x02, (tu_uint8) data[1]);
    ASSERT_EQ (0x03, (tu_uint8) data[2]);
}

TEST_F(BytesAppender, TestReadAndWriteAppender)
{
    tempo_utils::BytesAppender appender;
    tempo_utils::BytesAppender src;
    src.appendU8(0x01);
    src.appendU8(0x02);
    src.appendU8(0x03);
    appender.appendBytes(src);
    auto bytes = appender.finish();
    ASSERT_TRUE (bytes != nullptr);
    ASSERT_EQ (3, bytes->getSize());
    auto *data = bytes->getData();
    ASSERT_EQ (0x01, (tu_uint8) data[0]);
    ASSERT_EQ (0x02, (tu_uint8) data[1]);
    ASSERT_EQ (0x03, (tu_uint8) data[2]);
}
