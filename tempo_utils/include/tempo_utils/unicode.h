#ifndef TEMPO_UTILS_UNICODE_H
#define TEMPO_UTILS_UNICODE_H

#include <string>

#include <absl/strings/string_view.h>
#include <unicode/ustring.h>

#include "integer_types.h"

namespace tempo_utils {

    typedef bool (*IsUnreservedCharFunc)(char);

    /**
     * @brief Convert a single UTF32 character into a UTF-8 string.
     *
     * @param utf8 The UTF32 encoded source character.
     * @return The UTF-8 encoded string.
     */
    std::string convert_to_utf8(UChar32 chr);

    /**
     * @brief Convert a UTF-16 string into a UTF-8 string.
     *
     * @param The UTF-16 encoded source string.
     * @return utf8 The UTF-8 encoded string.
     */
    std::string convert_to_utf8(const UChar *utf16, tu_int32 size);

    std::string unescape_utf8(std::string_view utf8);

    /**
     * @brief Percent-encode a UTF-8 string using the default unreserved character set.
     *
     * The default unreserved character set contains ASCII alphanumeric characters,
     * the dash character '-', the period character '.', the underscore character '_',
     * and the tilde character '~'. All other octets will be percent-encoded.
     *
     * @param utf8 The UTF-8 string to encode.
     * @return A UTF-8 string with all reserved characters encoded.
     */
    std::string percent_encode_utf8(std::string_view utf8);

    /**
     * @brief Percent-encode a UTF-8 string using the given func to identify unreserved characters.
     *
     * @param utf8 The UTF-8 string to encode.
     * @param func A function which returns true if the specified char is unreserved, otherwise false.
     * @return A UTF-8 string with all reserved characters encoded.
     */
    std::string percent_encode_utf8(std::string_view utf8, IsUnreservedCharFunc func);

    /**
     * @brief Decode a percent-encoded UTF-8 string.
     *
     * @param utf8 The percent-encoded UTF-8 string.
     * @return A UTF-8 string with all percent-encoded octets decoded.
     */
    std::string percent_decode_utf8(std::string_view utf8);

    /**
     * @brief Convert a UTF-8 string into a UTF-16 string.
     *
     * @param utf8 The UTF-8 encoded source string.
     * @return The UTF-16 encoded string.
     */
    std::u16string convert_to_utf16(std::string_view utf8);
};

#endif // TEMPO_UTILS_UNICODE_H
