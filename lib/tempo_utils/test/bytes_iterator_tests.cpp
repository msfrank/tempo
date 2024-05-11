
#include <gtest/gtest.h>

#include <tempo_utils/bytes_appender.h>
#include <tempo_utils/bytes_iterator.h>

TEST(BytesIterator, TestReadU8)
{
    tempo_utils::BytesAppender appender;
    appender.appendU8(42);
    auto bytes = appender.finish();

    tempo_utils::BytesIterator iterator(bytes);
    ASSERT_TRUE (iterator.isValid());

    tu_uint8 u8;
    ASSERT_TRUE (iterator.nextU8(u8));
    ASSERT_EQ (42, u8);

    ASSERT_FALSE (iterator.isValid());
}

TEST(BytesIterator, TestReadU16)
{
    tempo_utils::BytesAppender appender;
    appender.appendU16(12345);
    auto bytes = appender.finish();

    tempo_utils::BytesIterator iterator(bytes);
    ASSERT_TRUE (iterator.isValid());

    tu_uint16 u16;
    ASSERT_TRUE (iterator.nextU16(u16));
    ASSERT_EQ (12345, u16);

    ASSERT_FALSE (iterator.isValid());
}

TEST(BytesIterator, TestReadU32)
{
    tempo_utils::BytesAppender appender;
    appender.appendU32(123456789);
    auto bytes = appender.finish();

    tempo_utils::BytesIterator iterator(bytes);
    ASSERT_TRUE (iterator.isValid());

    tu_uint32 u32;
    ASSERT_TRUE (iterator.nextU32(u32));
    ASSERT_EQ (123456789, u32);

    ASSERT_FALSE (iterator.isValid());
}

TEST(BytesIterator, TestReadU64)
{
    tempo_utils::BytesAppender appender;
    appender.appendU64(123456789012345);
    auto bytes = appender.finish();

    tempo_utils::BytesIterator iterator(bytes);
    ASSERT_TRUE (iterator.isValid());

    tu_uint64 u64;
    ASSERT_TRUE (iterator.nextU64(u64));
    ASSERT_EQ (123456789012345, u64);

    ASSERT_FALSE (iterator.isValid());
}

TEST(BytesIterator, TestReadSlice)
{
    tempo_utils::BytesAppender appender;
    appender.appendBytes("hello, world!");
    auto bytes = appender.finish();

    tempo_utils::BytesIterator iterator(bytes);
    ASSERT_TRUE (iterator.isValid());

    tempo_utils::Slice slice;
    ASSERT_TRUE (iterator.nextSlice(slice, 13));
    ASSERT_EQ ("hello, world!", std::string_view((const char *) slice.getData(), slice.getSize()));

    ASSERT_FALSE (iterator.isValid());
}
