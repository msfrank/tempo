#ifndef TEMPO_UTILS_BYTES_APPENDER_H
#define TEMPO_UTILS_BYTES_APPENDER_H

#include <string>

#include "integer_types.h"
#include "memory_bytes.h"
#include "result.h"

namespace tempo_utils {

    class BytesAppender {

    public:
        explicit BytesAppender(int initialCapacity = 0);
        BytesAppender(const BytesAppender &other) = delete;

        void appendU8(tu_uint8 u8);
        void appendU16(tu_uint16 u16);
        void appendU32(tu_uint32 u32);
        void appendU64(tu_uint64 u64);
        void appendBytes(std::span<const tu_uint8> bytes);
        void appendBytes(std::string_view str);
        void appendBytes(const BytesAppender &appender);

        const tu_uint8 *getData() const;
        tu_uint32 getSize() const;
        std::vector<tu_uint8>::const_iterator bytesBegin() const;
        std::vector<tu_uint8>::const_iterator bytesEnd() const;

        std::shared_ptr<const MemoryBytes> finish();

    private:
        std::unique_ptr<std::vector<tu_uint8>> m_bytes;
    };
}

#endif // TEMPO_UTILS_BYTES_APPENDER_H
