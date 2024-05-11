#ifndef TEMPO_UTILS_BYTES_ITERATOR_H
#define TEMPO_UTILS_BYTES_ITERATOR_H

#include <memory>

#include "immutable_bytes.h"
#include "slice.h"

namespace tempo_utils {

    class BytesIterator {
    public:
        explicit BytesIterator(std::shared_ptr<const ImmutableBytes> bytes);

        bool isValid() const;

        bool nextU8(tu_uint8 &u8);
        bool nextU16(tu_uint16 &u16);
        bool nextU32(tu_uint32 &u32);
        bool nextU64(tu_uint64 &u64);
        bool nextSlice(Slice &slice, tu_uint32 count);

    private:
        std::shared_ptr<const ImmutableBytes> m_bytes;
        tu_uint32 m_offset;
    };
}

#endif // TEMPO_UTILS_BYTES_ITERATOR_H
