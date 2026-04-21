
#include <gtest/gtest.h>

#include <tempo_utils/bytes_appender.h>
#include <tempo_utils/bytes_iterator.h>

class BytesIterator : public ::testing::Test {};

TEST_F(BytesIterator, TestReadU8)
{
    tempo_utils::BytesAppender appender;
    appender.appendU8(42);
    auto bytes = appender.finish();

    tempo_utils::BytesIterator iterator(bytes->getSpan());
    ASSERT_TRUE (iterator.isValid());

    tu_uint8 u8;
    ASSERT_TRUE (iterator.nextU8(u8));
    ASSERT_EQ (42, u8);

    ASSERT_FALSE (iterator.isValid());
}

TEST_F(BytesIterator, TestReadU16)
{
    tempo_utils::BytesAppender appender;
    appender.appendU16(12345);
    auto bytes = appender.finish();

    tempo_utils::BytesIterator iterator(bytes->getSpan());
    ASSERT_TRUE (iterator.isValid());

    tu_uint16 u16;
    ASSERT_TRUE (iterator.nextU16(u16));
    ASSERT_EQ (12345, u16);

    ASSERT_FALSE (iterator.isValid());
}

TEST_F(BytesIterator, TestReadU16LE)
{
    tempo_utils::BytesAppender appender;
    appender.appendU16LE(12345);
    auto bytes = appender.finish();

    tempo_utils::BytesIterator iterator(bytes->getSpan());
    ASSERT_TRUE (iterator.isValid());

    tu_uint16 u16;
    ASSERT_TRUE (iterator.nextU16LE(u16));
    ASSERT_EQ (12345, u16);

    ASSERT_FALSE (iterator.isValid());
}

TEST_F(BytesIterator, TestReadU32)
{
    tempo_utils::BytesAppender appender;
    appender.appendU32(123456789);
    auto bytes = appender.finish();

    tempo_utils::BytesIterator iterator(bytes->getSpan());
    ASSERT_TRUE (iterator.isValid());

    tu_uint32 u32;
    ASSERT_TRUE (iterator.nextU32(u32));
    ASSERT_EQ (123456789, u32);

    ASSERT_FALSE (iterator.isValid());
}

TEST_F(BytesIterator, TestReadU32LE)
{
    tempo_utils::BytesAppender appender;
    appender.appendU32LE(123456789);
    auto bytes = appender.finish();

    tempo_utils::BytesIterator iterator(bytes->getSpan());
    ASSERT_TRUE (iterator.isValid());

    tu_uint32 u32;
    ASSERT_TRUE (iterator.nextU32LE(u32));
    ASSERT_EQ (123456789, u32);

    ASSERT_FALSE (iterator.isValid());
}

TEST_F(BytesIterator, TestReadU64)
{
    tempo_utils::BytesAppender appender;
    appender.appendU64(123456789012345);
    auto bytes = appender.finish();

    tempo_utils::BytesIterator iterator(bytes->getSpan());
    ASSERT_TRUE (iterator.isValid());

    tu_uint64 u64;
    ASSERT_TRUE (iterator.nextU64(u64));
    ASSERT_EQ (123456789012345, u64);

    ASSERT_FALSE (iterator.isValid());
}

TEST_F(BytesIterator, TestReadU64LE)
{
    tempo_utils::BytesAppender appender;
    appender.appendU64LE(123456789012345);
    auto bytes = appender.finish();

    tempo_utils::BytesIterator iterator(bytes->getSpan());
    ASSERT_TRUE (iterator.isValid());

    tu_uint64 u64;
    ASSERT_TRUE (iterator.nextU64LE(u64));
    ASSERT_EQ (123456789012345, u64);

    ASSERT_FALSE (iterator.isValid());
}

TEST_F(BytesIterator, TestReadF32)
{
    tempo_utils::BytesAppender appender;
    appender.appendF32(123456.789);
    auto bytes = appender.finish();

    tempo_utils::BytesIterator iterator(bytes->getSpan());
    ASSERT_TRUE (iterator.isValid());

    float f32;
    ASSERT_TRUE (iterator.nextF32(f32));
    ASSERT_FLOAT_EQ (123456.789, f32);

    ASSERT_FALSE (iterator.isValid());
}

TEST_F(BytesIterator, TestReadF32LE)
{
    tempo_utils::BytesAppender appender;
    appender.appendF32LE(123456.789);
    auto bytes = appender.finish();

    tempo_utils::BytesIterator iterator(bytes->getSpan());
    ASSERT_TRUE (iterator.isValid());

    float f32;
    ASSERT_TRUE (iterator.nextF32LE(f32));
    ASSERT_FLOAT_EQ (123456.789, f32);

    ASSERT_FALSE (iterator.isValid());
}

TEST_F(BytesIterator, TestReadF64)
{
    tempo_utils::BytesAppender appender;
    appender.appendF64(12345645678912345.4321);
    auto bytes = appender.finish();

    tempo_utils::BytesIterator iterator(bytes->getSpan());
    ASSERT_TRUE (iterator.isValid());

    double f64;
    ASSERT_TRUE (iterator.nextF64(f64));
    ASSERT_DOUBLE_EQ (12345645678912345.4321, f64);

    ASSERT_FALSE (iterator.isValid());
}

TEST_F(BytesIterator, TestReadF64LE)
{
    tempo_utils::BytesAppender appender;
    appender.appendF64LE(12345645678912345.4321);
    auto bytes = appender.finish();

    tempo_utils::BytesIterator iterator(bytes->getSpan());
    ASSERT_TRUE (iterator.isValid());

    double f64;
    ASSERT_TRUE (iterator.nextF64LE(f64));
    ASSERT_DOUBLE_EQ (12345645678912345.4321, f64);

    ASSERT_FALSE (iterator.isValid());
}

TEST_F(BytesIterator, TestReadSlice)
{
    tempo_utils::BytesAppender appender;
    appender.appendBytes("hello, world!");
    auto bytes = appender.finish();

    tempo_utils::BytesIterator iterator(bytes->getSpan());
    ASSERT_TRUE (iterator.isValid());

    std::span<const tu_uint8> slice;
    ASSERT_TRUE (iterator.nextSlice(slice, 13));
    ASSERT_EQ ("hello, world!", std::string_view((const char *) slice.data(), slice.size()));

    ASSERT_FALSE (iterator.isValid());
}
