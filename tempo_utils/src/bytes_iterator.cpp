
#include <tempo_utils/big_endian.h>
#include <tempo_utils/bytes_iterator.h>
#include <tempo_utils/log_stream.h>

/**
 * Construct a new iterator over the given sequence of bytes.
 *
 * @param bytes The bytes to iterate over.
 */
tempo_utils::BytesIterator::BytesIterator(std::shared_ptr<const ImmutableBytes> bytes)
    : m_bytes(bytes),
      m_offset(0)
{
    TU_ASSERT (m_bytes != nullptr);
}

/**
 * Returns true if the iterator is still valid, otherwise false if the iterator has reached the end of the bytes.
 *
 * @return true if the iterator is still valid, otherwise false.
 */
bool
tempo_utils::BytesIterator::isValid() const
{
    return m_offset < m_bytes->getSize();
}

inline bool valid_request(tu_uint32 size, tu_uint32 offset, tu_uint32 requested)
{
    return (size - offset) >= requested;
}

/**
 * If the iterator is valid and has at least 1 byte available to be consumed then store the value at the
 * cursor in `u8` and move the cursor forward by 1 byte.
 *
 * @param u8 The location to receive the next uint8.
 * @return true if the iteration was successful, otherwise false.
 */
bool
tempo_utils::BytesIterator::nextU8(tu_uint8 &u8)
{
    if (!valid_request(m_bytes->getSize(), m_offset, 1))
        return false;
    auto *data = m_bytes->getData();
    u8 = data[m_offset];
    m_offset++;
    return true;
}

/**
 * If the iterator is valid and has at least 2 bytes available to be consumed then store the value at the
 * cursor in `u16` and move the cursor forward by 2 bytes.
 *
 * @param u16 The location to receive the next uint16.
 * @return true if the iteration was successful, otherwise false.
 */
bool
tempo_utils::BytesIterator::nextU16(tu_uint16 &u16)
{
    if (!valid_request(m_bytes->getSize(), m_offset, 2))
        return false;
    auto *data = m_bytes->getData();
    u16 = read_u16(data + m_offset);
    m_offset += 2;
    return true;
}

/**
 * If the iterator is valid and has at least 4 bytes available to be consumed then store the value at the
 * cursor in `u32` and move the cursor forward by 4 bytes.
 *
 * @param u32 The location to receive the next uint32.
 * @return true if the iteration was successful, otherwise false.
 */
bool
tempo_utils::BytesIterator::nextU32(tu_uint32 &u32)
{
    if (!valid_request(m_bytes->getSize(), m_offset, 4))
        return false;
    auto *data = m_bytes->getData();
    u32 = read_u32(data + m_offset);
    m_offset += 4;
    return true;
}

/**
 * If the iterator is valid and has at least 8 bytes available to be consumed then store the value at the
 * cursor in `u64` and move the cursor forward by 8 bytes.
 *
 * @param u64 The location to receive the next uint64.
 * @return true if the iteration was successful, otherwise false.
 */
bool
tempo_utils::BytesIterator::nextU64(tu_uint64 &u64)
{
    if (!valid_request(m_bytes->getSize(), m_offset, 8))
        return false;
    auto *data = m_bytes->getData();
    u64 = read_u64(data + m_offset);
    m_offset += 8;
    return true;
}

/**
 * If the iterator is valid and has at least `count` bytes available to be consumed then store the value at the
 * cursor in `slice` and move the cursor forward by `count` bytes.
 *
 * @param slice The location to receive the slice.
 * @return true if the iteration was successful, otherwise false.
 */
bool
tempo_utils::BytesIterator::nextSlice(Slice &slice, tu_uint32 count)
{
    if (!valid_request(m_bytes->getSize(), m_offset, count))
        return false;
    slice = Slice(m_bytes, m_offset, count);
    m_offset += count;
    return true;
}