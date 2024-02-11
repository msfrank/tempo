
#include <gtest/gtest.h>

#include <tempo_utils/big_endian.h>

TEST(BigEndian, TestReadAndWriteU16)
{
    tu_uint8 src[2] = {0, 0};
    tempo_utils::write_u16(0x55AA, src);
    ASSERT_EQ (0x55, src[0]);
    ASSERT_EQ (0xAA, src[1]);
    const tu_uint8 *ptr = src;
    ASSERT_EQ (0x55AA, tempo_utils::read_u16(ptr));
    tu_uint16 dst = tempo_utils::read_u16_and_advance(ptr);
    ASSERT_EQ (0x55AA, dst);
    ASSERT_EQ (src + 2, ptr);
}

TEST(BigEndian, TestReadAndWriteI16)
{
    tu_uint8 src[2] = {0, 0};
    tempo_utils::write_i16(-12345, src);
    ASSERT_EQ (0xCF, src[0]);
    ASSERT_EQ (0xC7, src[1]);
    const tu_uint8 *ptr = src;
    ASSERT_EQ (-12345, tempo_utils::read_i16(ptr));
    tu_int16 dst = tempo_utils::read_i16_and_advance(ptr);
    ASSERT_EQ (-12345, dst);
    ASSERT_EQ (src + 2, ptr);
}

TEST(BigEndian, TestReadAndWriteU32)
{
    tu_uint8 src[4] = {0, 0, 0, 0};
    tempo_utils::write_u32(0x55AA9922, src);
    ASSERT_EQ (0x55, src[0]);
    ASSERT_EQ (0xAA, src[1]);
    ASSERT_EQ (0x99, src[2]);
    ASSERT_EQ (0x22, src[3]);
    const tu_uint8 *ptr = src;
    ASSERT_EQ (0x55AA9922, tempo_utils::read_u32(ptr));
    tu_uint32 dst = tempo_utils::read_u32_and_advance(ptr);
    ASSERT_EQ (0x55AA9922, dst);
    ASSERT_EQ (src + 4, ptr);
}

TEST(BigEndian, TestReadAndWriteI32)
{
    tu_uint8 src[4] = {0, 0, 0, 0};
    tempo_utils::write_i32(-1234509876, src);
    ASSERT_EQ (0xB6, src[0]);
    ASSERT_EQ (0x6A, src[1]);
    ASSERT_EQ (0xDF, src[2]);
    ASSERT_EQ (0xCC, src[3]);
    const tu_uint8 *ptr = src;
    ASSERT_EQ (-1234509876, tempo_utils::read_i32(ptr));
    tu_int32 dst = tempo_utils::read_i32_and_advance(ptr);
    ASSERT_EQ (-1234509876, dst);
    ASSERT_EQ (src + 4, ptr);
}

TEST(BigEndian, TestReadAndWriteU64)
{
    tu_uint8 src[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    tempo_utils::write_u64(0x55AA992212345678, src);
    ASSERT_EQ (0x55, src[0]);
    ASSERT_EQ (0xAA, src[1]);
    ASSERT_EQ (0x99, src[2]);
    ASSERT_EQ (0x22, src[3]);
    ASSERT_EQ (0x12, src[4]);
    ASSERT_EQ (0x34, src[5]);
    ASSERT_EQ (0x56, src[6]);
    ASSERT_EQ (0x78, src[7]);
    const tu_uint8 *ptr = src;
    ASSERT_EQ (0x55AA992212345678, tempo_utils::read_u64(ptr));
    tu_uint64 dst = tempo_utils::read_u64_and_advance(ptr);
    ASSERT_EQ (0x55AA992212345678, dst);
    ASSERT_EQ (src + 8, ptr);
}

TEST(BigEndian, TestReadAndWriteI64)
{
    tu_uint8 src[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    tempo_utils::write_i64(-1234509876246897531, src);
    ASSERT_EQ (0xEE, src[0]);
    ASSERT_EQ (0xDE, src[1]);
    ASSERT_EQ (0x23, src[2]);
    ASSERT_EQ (0xCE, src[3]);
    ASSERT_EQ (0xEA, src[4]);
    ASSERT_EQ (0x97, src[5]);
    ASSERT_EQ (0x9C, src[6]);
    ASSERT_EQ (0x85, src[7]);
    const tu_uint8 *ptr = src;
    ASSERT_EQ (-1234509876246897531, tempo_utils::read_i64(ptr));
    tu_int64 dst = tempo_utils::read_i64_and_advance(ptr);
    ASSERT_EQ (-1234509876246897531, dst);
    ASSERT_EQ (src + 8, ptr);
}