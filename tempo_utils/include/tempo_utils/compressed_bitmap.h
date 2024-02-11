#ifndef TEMPO_UTILS_COMPRESSED_BITMAP_H
#define TEMPO_UTILS_COMPRESSED_BITMAP_H

#include <cstdint>
#include <string>
#include <vector>

#include "integer_types.h"

namespace tempo_utils {

    class CompressedBitmap {

    public:
        CompressedBitmap();
        explicit CompressedBitmap(tu_uint32 capacity);
        CompressedBitmap(const CompressedBitmap &other);
        ~CompressedBitmap();

        bool isValid() const;

        bool contains(tu_uint32 value) const;
        tu_uint32 min() const;
        tu_uint32 max() const;
        tu_uint64 count() const;
        bool isEmpty() const;

        bool equals(const CompressedBitmap &other);
        bool isSubsetOf(const CompressedBitmap &other);

        void add(tu_uint32 value);
        void addRange(tu_uint32 start, tu_uint32 count);
        void addMany(tu_uint32 *data, size_t size);
        tu_uint32 reserve();
        void remove(tu_uint32 value);
        void clear();

        void print();

        std::string serialize() const;

        static CompressedBitmap invalid();
        static CompressedBitmap deserialize(const std::string &bytes);

    private:
        void *m_bitmap;

        explicit CompressedBitmap(void *bitmap);
    };
}

#endif // TEMPO_UTILS_COMPRESSED_BITMAP_H
