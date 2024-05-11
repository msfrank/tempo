#ifndef TEMPO_UTILS_URL_TYPES_H
#define TEMPO_UTILS_URL_TYPES_H

#include "integer_types.h"

namespace tempo_utils {

    constexpr tu_uint16 kHasScheme              = 0x0001 << 0;
    constexpr tu_uint16 kHasHost                = 0x0001 << 1;
    constexpr tu_uint16 kHasPort                = 0x0001 << 2;
    constexpr tu_uint16 kHasUsername            = 0x0001 << 3;
    constexpr tu_uint16 kHasPassword            = 0x0001 << 4;
    constexpr tu_uint16 kHasPath                = 0x0001 << 5;
    constexpr tu_uint16 kHasQuery               = 0x0001 << 6;
    constexpr tu_uint16 kHasFragment            = 0x0001 << 7;

    enum class KnownUrlScheme {
        Unknown,
        NoScheme,
        Http,
        Https,
        File,
    };

    namespace internal {
        struct UrlData;
    }
}

#endif // TEMPO_UTILS_URL_TYPES_H
