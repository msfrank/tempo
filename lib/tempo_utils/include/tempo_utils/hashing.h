#ifndef TEMPO_UTILS_HASHING_H
#define TEMPO_UTILS_HASHING_H

#include <stddef.h>

namespace tempo_utils {

    constexpr int kHashBits = sizeof(size_t) * 8;

    size_t hash_combine(size_t seed, size_t value);
}

#endif // TEMPO_UTILS_HASHING_H
