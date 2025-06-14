
#include <utf8.h>

#include <tempo_utils/integer_types.h>
#include <tempo_utils/unicode.h>

/**
 * Get the code point at the specified index in the UTF-8 encoded string. if the string `utf8` is empty,
 * or `index` is out of bounds for the string, or if the string contains invalid code points, then the
 * null terminator \0 is returned.
 *
 * @param utf8
 * @param index
 * @return
 */
char32_t
tempo_utils::get_utf8_char(std::string_view utf8, tu_uint32 index)
{
    if (utf8.empty() || utf8.size() <= index)
        return 0;

    try {
        auto it = utf8.begin();
        auto end = utf8.end();
        utf8::advance(it, index, end);
        return utf8::peek_next(it, end);
    } catch (utf8::not_enough_room &ex) {
        return 0;
    } catch (utf8::invalid_code_point &ex) {
        return 0;
    }
}

/**
 * Get the number of code points in the UTF-8 encoded string `utf8`. If the string contains invalid code
 * points then 0 is returned.
 *
 * @param utf8
 * @return
 */
tu_uint32
tempo_utils::get_utf8_length(std::string_view utf8)
{
    if (utf8.empty())
        return 0;

    try {
        auto it = utf8.begin();
        auto end = utf8.end();
        tu_int32 length = 0;
        while (it != end) {
            utf8::next(it, end);
            length++;
        }
        return length;
    } catch (utf8::invalid_code_point &ex) {
        return 0;
    }
}

/**
 * Convert a single UTF32 encoded character into a UTF-8 string. If `chr` is not a valid code
 * point then an empty string is returned.
 *
 * @param chr The UTF32 encoded source character.
 * @return The UTF-8 encoded string.
 */
std::string
tempo_utils::convert_to_utf8(char32_t chr)
{
    try {
        std::string utf8;
        utf8::append(chr, utf8);
        return utf8;
    } catch (utf8::invalid_code_point &ex) {
        return {};
    }
}

/**
 * Convert the UTF-16 encoded string `utf16` to a UTF-8 encoded string. If `utf16` contains any
 * invalid code points then an empty string is returned.
 *
 * @param utf16 The UTF-16 encoded source string.
 * @return The UTF-8 encoded string.
 */
std::string
tempo_utils::convert_to_utf8(std::u16string_view utf16)
{
    try {
        return utf8::utf16to8(utf16);
    } catch (utf8::invalid_code_point &ex) {
        return {};
    }
}

/**
 * Convert the UTF-8 encoded string `utf8` into a UTF-16 encoded string. If `utf8` contains any
 * invalid code points then an empty string is returned.
 *
 * @param utf8 The UTF-8 encoded source string.
 * @return The UTF-16 encoded string.
 */
std::u16string
tempo_utils::convert_to_utf16(std::string_view utf8)
{
    if (utf8.empty())
        return {};
    try {
        return utf8::utf8to16(utf8);
    } catch (utf8::invalid_code_point &ex) {
        return {};
    }
}

inline void
update_result(tu_uint32 value, int index, tu_uint32 &result)
{
    value = value << (index * 4);
    result |= value;
}

inline bool
next_hex_nybble(
    std::string_view::const_iterator &it,
    std::string_view::const_iterator &end,
    int index,
    tu_uint32 &result)
{
    if (it == end) return false;
    auto ch = utf8::next(it, end);
    switch (ch) {
        case U'0':  update_result(0, index, result); return true;
        case U'1':  update_result(1, index, result); return true;
        case U'2':  update_result(2, index, result); return true;
        case U'3':  update_result(3, index, result); return true;
        case U'4':  update_result(4, index, result); return true;
        case U'5':  update_result(5, index, result); return true;
        case U'6':  update_result(6, index, result); return true;
        case U'7':  update_result(7, index, result); return true;
        case U'8':  update_result(8, index, result); return true;
        case U'9':  update_result(9, index, result); return true;
        case U'A':
        case U'a':  update_result(10, index, result); return true;
        case U'B':
        case U'b':  update_result(11, index, result); return true;
        case U'C':
        case U'c':  update_result(12, index, result); return true;
        case U'D':
        case U'd':  update_result(13, index, result); return true;
        case U'E':
        case U'e':  update_result(14, index, result); return true;
        case U'F':
        case U'f':  update_result(15, index, result); return true;
        default:
            return false;
    }
}

inline bool
append_u32(
    std::string_view::const_iterator &it,
    std::string_view::const_iterator &end,
    std::string &result)
{
    tu_uint32 next = 0;
    if (!next_hex_nybble(it, end, 7, next)) return false;
    if (!next_hex_nybble(it, end, 6, next)) return false;
    if (!next_hex_nybble(it, end, 5, next)) return false;
    if (!next_hex_nybble(it, end, 4, next)) return false;
    if (!next_hex_nybble(it, end, 3, next)) return false;
    if (!next_hex_nybble(it, end, 2, next)) return false;
    if (!next_hex_nybble(it, end, 1, next)) return false;
    if (!next_hex_nybble(it, end, 0, next)) return false;
    auto ch = static_cast<char32_t>(next);
    utf8::append(ch, result);
    return true;
}

inline bool
append_u16(
    std::string_view::const_iterator &it,
    std::string_view::const_iterator &end,
    std::string &result)
{
    tu_uint32 next = 0;
    if (!next_hex_nybble(it, end, 3, next)) return false;
    if (!next_hex_nybble(it, end, 2, next)) return false;
    if (!next_hex_nybble(it, end, 1, next)) return false;
    if (!next_hex_nybble(it, end, 0, next)) return false;
    auto ch = static_cast<char32_t>(next);
    utf8::append(ch, result);
    return true;
}

inline bool
append_u8(
    std::string_view::const_iterator &it,
    std::string_view::const_iterator &end,
    std::string &result)
{
    tu_uint32 next = 0;
    if (!next_hex_nybble(it, end, 1, next)) return false;
    if (!next_hex_nybble(it, end, 0, next)) return false;
    auto ch = static_cast<char32_t>(next);
    utf8::append(ch, result);
    return true;
}

/**
 * Return a copy of the UTF-8 encoded string `utf8` with any escape sequences converted to their
 * unescaped value. If `utf8` contains any invalid code points then an empty string is returned.
 *
 * The following escape sequences are recognized:
 *   - ANSI C escapes: \a \b \t \n \v \f \r \" \' \? \\
 *   - GCC escape \e
 *   - 1 byte hexadecimal escape sequence \xhh where each 'h' is a hexadecimal digit
 *   - 2 byte universal character sequence \uhhhh where each 'h' is a hexadecimal digit
 *   - 4 byte universal character sequence \Uhhhhhhhh where each 'h' is a hexadecimal digit
 *
 * @param utf8
 * @return The unescaped UTF-8 encoded string.
 */
std::string
tempo_utils::unescape_utf8(std::string_view utf8)
{
    if (utf8.empty())
        return {};

    std::string unescaped;

    auto it = utf8.cbegin();
    auto end = utf8.cend();

    while (it != end) {
        try {
            auto ch = utf8::next(it, end);
            if (ch == U'\\') {
                if (it == end)
                    return {};  // incomplete escape

                ch = utf8::next(it, end);
                switch (ch) {
                    case U'x':
                        if (!append_u8(it, end, unescaped)) return {};
                        break;
                    case U'u':
                        if (!append_u16(it, end, unescaped)) return {};
                        break;
                    case U'U':
                        if (!append_u32(it, end, unescaped)) return {};
                        break;
                    case U'a':      utf8::append(U'\a', unescaped); break;      // audible bell
                    case U'b':      utf8::append(U'\b', unescaped); break;      // backspace
                    case U'e':      utf8::append(U'\e', unescaped); break;      // escape
                    case U'f':      utf8::append(U'\f', unescaped); break;      // form feed
                    case U'n':      utf8::append(U'\n', unescaped); break;      // newline
                    case U'r':      utf8::append(U'\r', unescaped); break;      // carriage return
                    case U't':      utf8::append(U'\t', unescaped); break;      // horizontal tab
                    case U'v':      utf8::append(U'\v', unescaped); break;      // vertical tab
                    case U'\\':     utf8::append(U'\\', unescaped); break;      // backslash
                    case U'\'':     utf8::append(U'\'', unescaped); break;      // single quote
                    case U'\"':     utf8::append(U'\"', unescaped); break;      // double quote
                    case U'\?':     utf8::append(U'\?', unescaped); break;      // question mark
                    default:
                        return {};
                }

            } else {
                utf8::append(ch, unescaped);
            }
        } catch (utf8::invalid_code_point &ex) {
            return {};
        }
    }
    return unescaped;
}

inline bool is_unreserved_char(char ch)
{
    return isalnum(ch) || ch == '-' || ch == '.' || ch == '_' || ch == '~';
}

inline void put_percent_encoded(char ch, std::string &s)
{
    char encoded[3] = { '%', '\0', '\0' };
    unsigned char hi = ((unsigned char) ch / 16);
    unsigned char lo = ((unsigned char) ch % 16);

    encoded[1] = hi < 10? '0' + hi : 'A' + (hi - 10);
    encoded[2] = lo < 10? '0' + lo : 'A' + (lo - 10);
    s.append(encoded, 3);
}

inline void put_percent_decoded(char hex[2], std::string &s)
{
    unsigned char hi = hex[0] < ':'? hex[0] - '0' : 10 + (hex[0] - 'A');
    unsigned char lo = hex[1] < ':'? hex[1] - '0' : 10 + (hex[1] - 'A');
    unsigned char ch = (hi * 16) + lo;
    s.push_back((char) ch);
}

/**
 * Percent-encode a UTF-8 string using the default unreserved character set.
 *
 * The default unreserved character set contains ASCII alphanumeric characters,
 * the dash character '-', the period character '.', the underscore character '_',
 * and the tilde character '~'. All other octets will be percent-encoded.
 *
 * @param utf8 The UTF-8 string to encode.
 * @return A UTF-8 string with all reserved characters encoded.
 */
std::string
tempo_utils::percent_encode_utf8(std::string_view utf8)
{
    std::string encoded;

    for (const char ch : utf8) {
        if (is_unreserved_char(ch)) {
            encoded.push_back(ch);
        } else {
            put_percent_encoded(ch, encoded);
        }
    }

    return encoded;
}

/**
 * Percent-encode a UTF-8 string using the given func to identify unreserved characters.
 *
 * @param utf8 The UTF-8 string to encode.
 * @param func A function which returns true if the specified char is unreserved, otherwise false.
 * @return A UTF-8 string with all reserved characters encoded.
 */
std::string
tempo_utils::percent_encode_utf8(std::string_view utf8, IsUnreservedCharFunc func)
{
    std::string encoded;

    for (const char ch : utf8) {
        if (func(ch)) {
            encoded.push_back(ch);
        } else {
            put_percent_encoded(ch, encoded);
        }
    }

    return encoded;
}

/**
 * Decode a percent-encoded UTF-8 string.
 *
 * @param utf8 The percent-encoded UTF-8 string.
 * @return A UTF-8 string with all percent-encoded octets decoded.
 */
std::string
tempo_utils::percent_decode_utf8(std::string_view utf8)
{
    std::string decoded;
    char hex[2];

    auto iterator = utf8.cbegin();
    while (iterator != utf8.cend()) {
        const auto ch = *iterator;
        if (ch == '%') {
            iterator++;
            if (iterator == utf8.cend())            // utf8 is truncated
                return {};
            if (*iterator == '%') {                 // next octet is a literal '%'
                decoded.push_back('%');
            }  else {                               // decode percent-encoded octet
                hex[0] = *iterator;
                iterator++;
                if (iterator == utf8.cend())        // utf8 is truncated
                    return {};
                hex[1] = *iterator;
                put_percent_decoded(hex, decoded);
            }
        } else {                                    // no decoding is necessary
            decoded.push_back(ch);
        }
        iterator++;
    }

    return decoded;
}