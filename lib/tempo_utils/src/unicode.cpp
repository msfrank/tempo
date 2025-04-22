
#include <tempo_utils/unicode.h>

std::string
tempo_utils::convert_to_utf8(UChar32 chr)
{
    UErrorCode err = U_ZERO_ERROR;

    // convert the uchar32 to a utf16 unterminated string
    UChar utf16[4] = {0, 0, 0, 0};
    int32_t nwritten16 = 0;
    u_strFromUTF32(utf16, 4, &nwritten16, &chr, 1, &err);
    if (err != U_ZERO_ERROR && err != U_BUFFER_OVERFLOW_ERROR)
        return {};

    err = U_ZERO_ERROR;

    // convert the utf16 unterminated string to a utf8 zero-terminated string
    char utf8[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    int32_t nwritten8 = 0;
    u_strToUTF8(utf8, 8, &nwritten8, utf16, nwritten16, &err);
    if (err != U_ZERO_ERROR && err != U_STRING_NOT_TERMINATED_WARNING)
        return {};

    return std::string(utf8, nwritten8);
}

std::string
tempo_utils::convert_to_utf8(const UChar *utf16, int32_t size)
{
    if (utf16 == nullptr || size <= 0)
        return {};

    UErrorCode err = U_ZERO_ERROR;

    // perform preflight to determine size of dst buffer
    int32_t dstSize = -1;
    u_strToUTF8(nullptr, 0, &dstSize, utf16, size, &err);
    if (err != U_ZERO_ERROR && err != U_BUFFER_OVERFLOW_ERROR)
        return {};

    err = U_ZERO_ERROR;

    // allocate destination string
    std::string utf8;
    utf8.resize(dstSize);           // NOTE: dstSize is in utf8 code units, which is a single byte
    char *dst = utf8.data();

    // convert utf16 to utf8
    int32_t nwritten;
    u_strToUTF8(dst, dstSize, &nwritten, utf16, size, &err);
    if (err != U_ZERO_ERROR && err != U_STRING_NOT_TERMINATED_WARNING)
        return {};

    return utf8;
}

std::u16string
tempo_utils::convert_to_utf16(std::string_view utf8)
{
    if (utf8.empty())
        return {};

    UErrorCode err = U_ZERO_ERROR;

    // perform preflight to determine size of dst buffer
    int32_t dstSize = -1;
    u_strFromUTF8(nullptr, 0, &dstSize, utf8.data(), utf8.size(), &err);
    if (err != U_ZERO_ERROR && err != U_BUFFER_OVERFLOW_ERROR)
        return {};

    err = U_ZERO_ERROR;

    // allocate destination string
    std::u16string utf16;
    utf16.resize(dstSize);           // NOTE: dstSize is count of UChars
    char16_t *dst = utf16.data();

    // convert utf16 to utf8
    int32_t nwritten;
    u_strFromUTF8(dst, dstSize, &nwritten, utf8.data(), utf8.size(), &err);
    if (err != U_ZERO_ERROR && err != U_STRING_NOT_TERMINATED_WARNING)
        return {};

    return utf16;
}

std::string
tempo_utils::unescape_utf8(std::string_view utf8)
{
    if (utf8.empty())
        return {};

    // perform preflight to determine size of dst buffer
    int32_t dstSize = -1;
    dstSize = u_unescape(utf8.data(), nullptr, 0);
    if (dstSize < 0)
        return {};

    // perform unescape on the string
    std::vector<UChar> dst(dstSize);
    auto nwritten = u_unescape(utf8.data(), dst.data(), dstSize);
    if (nwritten <= 0)
        return {};

    // return utf8 string
    return convert_to_utf8(dst.data(), dstSize);
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