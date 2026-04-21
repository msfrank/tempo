#ifndef TEMPO_UTILS_BYTES_ITERATOR_H
#define TEMPO_UTILS_BYTES_ITERATOR_H

#include <memory>

#include "immutable_bytes.h"
#include "slice.h"

namespace tempo_utils {

    class BytesIterator {
    public:
        explicit BytesIterator(std::span<const tu_uint8> bytes);
        BytesIterator(const tu_uint8 *data, size_t size);
        BytesIterator(const BytesIterator &other) = delete;

        bool isValid() const;

        bool nextU8(tu_uint8 &u8);
        bool nextS8(tu_int8 &s8);
        bool nextU16(tu_uint16 &u16);
        bool nextU16LE(tu_uint16 &u16);
        bool nextS16(tu_int16 &s16);
        bool nextS16LE(tu_int16 &s16);
        bool nextU32(tu_uint32 &u32);
        bool nextU32LE(tu_uint32 &u32);
        bool nextS32(tu_int32 &s32);
        bool nextS32LE(tu_int32 &s32);
        bool nextU64(tu_uint64 &u64);
        bool nextU64LE(tu_uint64 &u64);
        bool nextS64(tu_int64 &s64);
        bool nextS64LE(tu_int64 &s64);
        bool nextF32(float &f32);
        bool nextF32LE(float &f32);
        bool nextF64(double &f64);
        bool nextF64LE(double &f64);
        bool nextSlice(std::span<const tu_uint8> &slice, tu_uint32 count);

        size_t bytesConsumed() const;
        size_t bytesLeft() const;

    private:
        std::span<const tu_uint8> m_bytes;
        tu_uint32 m_offset;
    };
}

#endif // TEMPO_UTILS_BYTES_ITERATOR_H
