#ifndef TEMPO_UTILS_IMMUTABLE_BYTES_H
#define TEMPO_UTILS_IMMUTABLE_BYTES_H

#include "integer_types.h"

namespace tempo_utils {

    class ImmutableBytes {
    public:
        virtual ~ImmutableBytes() = default;
        virtual const tu_uint8 *getData() const = 0;
        virtual tu_uint32 getSize() const = 0;
    };
}

#endif // TEMPO_UTILS_IMMUTABLE_BYTES_H