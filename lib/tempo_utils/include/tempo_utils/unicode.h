#ifndef TEMPO_UTILS_UNICODE_H
#define TEMPO_UTILS_UNICODE_H

#include <string>

#include "integer_types.h"

namespace tempo_utils {

    typedef bool (*IsUnreservedCharFunc)(char);

    char32_t get_utf8_char(std::string_view utf8, tu_uint32 index);

    tu_uint32 get_utf8_length(std::string_view utf8);

    std::string convert_to_utf8(char32_t chr);

    std::string convert_to_utf8(std::u16string_view s);

    std::string unescape_utf8(std::string_view utf8);

    std::string percent_encode_utf8(std::string_view utf8);

    std::string percent_encode_utf8(std::string_view utf8, IsUnreservedCharFunc func);

    std::string percent_decode_utf8(std::string_view utf8);

    std::u16string convert_to_utf16(std::string_view utf8);
};

#endif // TEMPO_UTILS_UNICODE_H
