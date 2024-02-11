#include <gtest/gtest.h>

#include <tempo_utils/tempo_utils.h>

TEST(Unicode, TestPercentEncoding)
{
    auto encoded = tempo_utils::percent_encode_utf8("hello world!");

    ASSERT_EQ (encoded, "hello%20world%21");
}

TEST(Unicode, TestPercentDecoding)
{
    auto decoded = tempo_utils::percent_decode_utf8("hello%20world%21");

    ASSERT_EQ (decoded, "hello world!");
}