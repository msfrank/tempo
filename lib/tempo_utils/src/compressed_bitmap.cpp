
#include <roaring/roaring.h>

#include <tempo_utils/compressed_bitmap.h>
#include <tempo_utils/log_stream.h>

tempo_utils::CompressedBitmap::CompressedBitmap()
{
    m_bitmap = roaring_bitmap_create();
    TU_ASSERT(m_bitmap);
}

tempo_utils::CompressedBitmap::CompressedBitmap(tu_uint32 capacity)
{
    if (capacity > 0) {
        m_bitmap = roaring_bitmap_create_with_capacity(capacity);
    } else {
        m_bitmap = roaring_bitmap_create();
    }
    TU_ASSERT(m_bitmap);
}

tempo_utils::CompressedBitmap::CompressedBitmap(void *bitmap)
{
    TU_ASSERT(bitmap);
    m_bitmap = bitmap;
}

tempo_utils::CompressedBitmap::CompressedBitmap(const tempo_utils::CompressedBitmap &other)
{
    m_bitmap = roaring_bitmap_copy((roaring_bitmap_t *) other.m_bitmap);
    TU_ASSERT(m_bitmap);
}

tempo_utils::CompressedBitmap::~CompressedBitmap()
{
    if (m_bitmap)
        roaring_bitmap_free((roaring_bitmap_t *) m_bitmap);
}

bool
tempo_utils::CompressedBitmap::isValid() const
{
    return m_bitmap != nullptr;
}

bool
tempo_utils::CompressedBitmap::contains(tu_uint32 value) const
{
    if (m_bitmap == nullptr)
        return false;
    return roaring_bitmap_contains((roaring_bitmap_t *) m_bitmap, value);
}

tu_uint32
tempo_utils::CompressedBitmap::min() const
{
    if (m_bitmap == nullptr)
        return 0;
    return roaring_bitmap_minimum((roaring_bitmap_t *) m_bitmap);
}

tu_uint32
tempo_utils::CompressedBitmap::max() const
{
    if (m_bitmap == nullptr)
        return 0;
    return roaring_bitmap_maximum((roaring_bitmap_t *) m_bitmap);
}

tu_uint64
tempo_utils::CompressedBitmap::count() const
{
    if (m_bitmap == nullptr)
        return 0;
    return roaring_bitmap_get_cardinality((roaring_bitmap_t *) m_bitmap);
}

bool
tempo_utils::CompressedBitmap::isEmpty() const
{
    if (m_bitmap == nullptr)
        return true;
    return roaring_bitmap_is_empty((roaring_bitmap_t *) m_bitmap);
}

bool
tempo_utils::CompressedBitmap::equals(const CompressedBitmap &other)
{
    if (m_bitmap == nullptr && other.m_bitmap == nullptr)
        return true;
    if (m_bitmap != nullptr && other.m_bitmap != nullptr)
        return roaring_bitmap_equals((roaring_bitmap_t *) m_bitmap, (roaring_bitmap_t *) other.m_bitmap);
    return false;
}

bool
tempo_utils::CompressedBitmap::isSubsetOf(const CompressedBitmap &other)
{
    if (m_bitmap != nullptr && other.m_bitmap != nullptr)
        return roaring_bitmap_is_subset((roaring_bitmap_t *) m_bitmap, (roaring_bitmap_t *) other.m_bitmap);
    // TODO: is this correct if other bitmap is invalid?
    return false;
}

void
tempo_utils::CompressedBitmap::add(tu_uint32 value)
{
    if (m_bitmap == nullptr)
        return;
    roaring_bitmap_add((roaring_bitmap_t *) m_bitmap, value);
}
void
tempo_utils::CompressedBitmap::addRange(tu_uint32 start, tu_uint32 count)
{
    if (m_bitmap == nullptr)
        return;
    // FIXME: this is inefficient. v1.1.5 has add_range() call, we should upgrade
    for (tu_uint32 offset = 0; offset < count; offset++) {
        auto index = start + offset;
        if (index == std::numeric_limits<tu_uint32>::max())
            return;
        roaring_bitmap_add((roaring_bitmap_t *) m_bitmap, index);
    }
}

void
tempo_utils::CompressedBitmap::addMany(tu_uint32 *data, size_t size)
{
    if (m_bitmap == nullptr)
        return;
    roaring_bitmap_add_many((roaring_bitmap_t *) m_bitmap, size, data);
}

tu_uint32
tempo_utils::CompressedBitmap::reserve()
{
    TU_ASSERT(m_bitmap);

    roaring_uint32_iterator_t it;
    roaring_init_iterator((roaring_bitmap_t *) m_bitmap, &it);
    tu_uint32 reserved = 0;

    // loop until one of the following conditions is true:
    //   1. as a special case, when bitmap is empty reserved will be equal to 0.
    //   2. we found a hole in the bitmap.  in  this case, reserved will be the index of that hole.
    //   3. iterator has no more values.  in this case, reserved will be equal to max + 1.
    while (it.has_value) {
        if (it.current_value != reserved)
            break;
        roaring_advance_uint32_iterator(&it);
        reserved++;
        TU_ASSERT(reserved != UINT32_MAX);
    }

    add(reserved);
    return reserved;
//    tu_uint32 reserved;
//    if (isEmpty()) {
//        reserved = 0;
//    } else {
//        auto negation = roaring_bitmap_flip(m_bitmap, 0, std::numeric_limits<tu_uint64>::max() - 1);
//        TU_ASSERT(negation);
//        reserved = roaring_bitmap_minimum(negation);
//        roaring_bitmap_free(negation);
//    }
//    add(reserved);
//    return reserved;
}

void
tempo_utils::CompressedBitmap::remove(tu_uint32 value)
{
    if (m_bitmap == nullptr)
        return;
    roaring_bitmap_remove((roaring_bitmap_t *) m_bitmap, value);
}

void
tempo_utils::CompressedBitmap::clear()
{
    if (m_bitmap == nullptr)
        return;
    roaring_bitmap_clear((roaring_bitmap_t *) m_bitmap);
}

void
tempo_utils::CompressedBitmap::print()
{
    if (m_bitmap == nullptr)
        return;
    roaring_bitmap_printf((roaring_bitmap_t *) m_bitmap);
}

std::string
tempo_utils::CompressedBitmap::serialize() const
{
    if (m_bitmap == nullptr)
        return {};
    std::string bytes;
    size_t size = roaring_bitmap_size_in_bytes((roaring_bitmap_t *) m_bitmap);
    bytes.resize(static_cast<int>(size));
    char *data = bytes.data();
    size_t nwritten = roaring_bitmap_serialize((roaring_bitmap_t *) m_bitmap, data);
    if (nwritten != size)
        return std::string();
    return bytes;
}

tempo_utils::CompressedBitmap
tempo_utils::CompressedBitmap::invalid()
{
    return CompressedBitmap(nullptr);
}

tempo_utils::CompressedBitmap
tempo_utils::CompressedBitmap::deserialize(const std::string &bytes)
{
    if (bytes.empty())
        return CompressedBitmap::invalid();
    roaring_bitmap_t *bitmap = roaring_bitmap_deserialize(bytes.data());
    return CompressedBitmap(bitmap);
}
