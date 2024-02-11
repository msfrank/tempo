#ifndef TEMPO_UTILS_MEMORY_BYTES_H
#define TEMPO_UTILS_MEMORY_BYTES_H

#include <memory>
#include <span>
#include <string_view>
#include <vector>

#include "immutable_bytes.h"
#include "slice.h"

namespace tempo_utils {

    class MemoryBytes : public ImmutableBytes, public std::enable_shared_from_this<MemoryBytes> {
    public:
        static std::shared_ptr<MemoryBytes> create(std::vector<tu_uint8> &&bytes);
        static std::shared_ptr<MemoryBytes> copy(std::span<const tu_uint8> bytes);
        static std::shared_ptr<MemoryBytes> copy(std::initializer_list<tu_uint8> init);
        static std::shared_ptr<MemoryBytes> copy(std::string_view str);

        const tu_uint8 *getData() const override;
        tu_uint32 getSize() const override;
        std::span<const tu_uint8> getSpan() const;

        Slice toSlice() const;

    private:
        std::vector<tu_uint8> m_bytes;

        MemoryBytes() = default;
        explicit MemoryBytes(std::vector<tu_uint8> &&bytes);
    };
}

#endif // TEMPO_UTILS_MEMORY_BYTES_H
