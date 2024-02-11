#ifndef TEMPO_UTILS_MEMORY_MAPPED_BYTES_H
#define TEMPO_UTILS_MEMORY_MAPPED_BYTES_H

#include <filesystem>

#include "immutable_bytes.h"
#include "result.h"
#include "slice.h"

namespace tempo_utils {

    class MemoryMappedBytes : public ImmutableBytes, public std::enable_shared_from_this<MemoryMappedBytes> {
    public:
        static Result<std::shared_ptr<MemoryMappedBytes>> open(const std::filesystem::path &path);

        const tu_uint8 *getData() const override;
        tu_uint32 getSize() const override;
        std::span<const tu_uint8> getSpan() const;

        Slice toSlice() const;

    private:
        struct Priv;
        std::unique_ptr<Priv> m_priv;

        MemoryMappedBytes() = default;
        explicit MemoryMappedBytes(std::unique_ptr<Priv> &&priv);
    };
}

#endif // TEMPO_UTILS_MEMORY_MAPPED_BYTES_H
