
#include <gtest/gtest.h>

#include <tempo_utils/ieee754.h>

TEST(Ieee754, TestReadAndWriteDouble)
{
    tu_uint8 src[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    tempo_utils::write_dbl(543210.9876, src);
    ASSERT_EQ (0x41, src[0]);
    ASSERT_EQ (0x20, src[1]);
    ASSERT_EQ (0x93, src[2]);
    ASSERT_EQ (0xD5, src[3]);
    ASSERT_EQ (0xF9, src[4]);
    ASSERT_EQ (0xA6, src[5]);
    ASSERT_EQ (0xB5, src[6]);
    ASSERT_EQ (0x0B, src[7]);
    const tu_uint8 *ptr = src;
    ASSERT_DOUBLE_EQ(543210.9876, tempo_utils::read_dbl(ptr));
    double dst = tempo_utils::read_dbl_and_advance(ptr);
    ASSERT_EQ (543210.9876, dst);
    ASSERT_EQ (src + 8, ptr);
}
