#ifndef TEMPO_UTILS_SLICE_H
#define TEMPO_UTILS_SLICE_H

#include <memory>
#include <span>

#include "immutable_bytes.h"
#include "integer_types.h"

namespace tempo_utils {

    class Slice : public ImmutableBytes {
    public:
        Slice();
        explicit Slice(std::shared_ptr<const ImmutableBytes> bytes);
        Slice(std::shared_ptr<const ImmutableBytes> bytes, tu_uint32 pos);
        Slice(std::shared_ptr<const ImmutableBytes> bytes, tu_uint32 pos, tu_uint32 count);
        Slice(const Slice &other);
        Slice(Slice &&other) noexcept;

        Slice& operator=(const Slice &other);
        Slice& operator=(Slice &&other) noexcept;

        bool isEmpty() const;
        std::span<const tu_uint8> sliceView() const;
        tu_uint8 getByte(tu_uint32 index) const;

        const tu_uint8 *getData() const override;
        tu_uint32 getSize() const override;

        Slice slice(tu_uint32 pos, tu_uint32 count) const;

        std::shared_ptr<const ImmutableBytes> toImmutableBytes() const;

    private:
        std::shared_ptr<const ImmutableBytes> m_bytes;
        tu_uint32 m_pos;
        tu_uint32 m_count;
    };
}

#endif // TEMPO_UTILS_SLICE_H
