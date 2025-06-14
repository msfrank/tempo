#include <gtest/gtest.h>

#include <tempo_utils/tempo_utils.h>
#include <utf8/checked.h>

TEST(Unicode, GetCharAtIndex)
{
    auto string = utf8::utf32to8(std::u32string_view(U"abc人"));
    auto chr = tempo_utils::get_utf8_char(string, 3);
    ASSERT_EQ (U'人', chr);
}

TEST(Unicode, GetUtf8Length)
{
    auto string = utf8::utf32to8(std::u32string_view(U"abc人"));
    auto length = tempo_utils::get_utf8_length(string);
    ASSERT_EQ (4, length);
}


TEST(Unicode, ConvertChar32ToUtf8)
{
    char32_t chr = U'人';
    auto encoded = tempo_utils::convert_to_utf8(chr);
    ASSERT_FALSE (encoded.empty());
    auto *utf8 = encoded.c_str();
    auto res = utf8::next(utf8, utf8 + 4);
    ASSERT_EQ (res, chr);
}

TEST(Unicode, UnescapeStringWithNoEscapes)
{
    std::string noescapes = "hello world!";
    auto unescaped = tempo_utils::unescape_utf8(noescapes);
    ASSERT_EQ (noescapes, unescaped);
}

TEST(Unicode, UnescapeStringWithNewlineEscape)
{
    std::string escaped = R"(hello \"world\")";
    auto unescaped = tempo_utils::unescape_utf8(escaped);
    ASSERT_EQ ("hello \"world\"", unescaped);
}

TEST(Unicode, UnescapeStringWith1ByteHexEscape)
{
    std::string escaped = R"(hello \x22world\x22)";
    auto unescaped = tempo_utils::unescape_utf8(escaped);
    ASSERT_EQ ("hello \"world\"", unescaped);
}

TEST(Unicode, UnescapeStringWith2ByteHexEscape)
{
    std::string escaped = R"(hello \u0022world\u0022)";
    auto unescaped = tempo_utils::unescape_utf8(escaped);
    ASSERT_EQ ("hello \"world\"", unescaped);
}

TEST(Unicode, UnescapeStringWith4ByteHexEscape)
{
    std::string escaped = R"(hello \U00000022world\U00000022)";
    auto unescaped = tempo_utils::unescape_utf8(escaped);
    ASSERT_EQ ("hello \"world\"", unescaped);
}

TEST(Unicode, PercentEncodeString)
{
    auto encoded = tempo_utils::percent_encode_utf8("hello world!");

    ASSERT_EQ (encoded, "hello%20world%21");
}

TEST(Unicode, PercentDecodeString)
{
    auto decoded = tempo_utils::percent_decode_utf8("hello%20world%21");

    ASSERT_EQ (decoded, "hello world!");
}