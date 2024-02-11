
#include <gtest/gtest.h>

#include <tempo_utils/bytes_appender.h>

TEST(BytesAppender, TestReadAndWriteU8)
{
    tempo_utils::BytesAppender appender;
    appender.appendU8(0x42);
    auto bytes = appender.finish();
    ASSERT_TRUE (bytes != nullptr);
    ASSERT_EQ (1, bytes->getSize());
    auto *data = bytes->getData();
    ASSERT_EQ (0x42, (tu_uint8) data[0]);
}

TEST(BytesAppender, TestReadAndWriteU16)
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

TEST(BytesAppender, TestReadAndWriteU32)
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

TEST(BytesAppender, TestReadAndWriteStdSpan)
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

TEST(BytesAppender, TestReadAndWriteStdString)
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

TEST(BytesAppender, TestReadAndWriteStdStringView)
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
