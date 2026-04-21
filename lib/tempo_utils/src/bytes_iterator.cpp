
#include <boost/endian.hpp>

#include <tempo_utils/bytes_iterator.h>
#include <tempo_utils/log_stream.h>

/**
 * Construct a new iterator over the given sequence of bytes.
 *
 * @param bytes The bytes to iterate over.
 */
tempo_utils::BytesIterator::BytesIterator(std::span<const tu_uint8> bytes)
    : m_bytes(bytes),
      m_offset(0)
{
}

/**
 * Construct a new iterator over the given sequence of bytes.
 *
 * @param data The bytes to iterate over.
 * @param size the size of the byte sequence.
 */
tempo_utils::BytesIterator::BytesIterator(const tu_uint8 *data, size_t size)
    : m_offset(0)
{
    if (data != nullptr || size > 0) {
        m_bytes = std::span(data, size);
    }
}

/**
 * Returns true if the iterator is still valid, otherwise false if the iterator has reached the end of the bytes.
 *
 * @return true if the iterator is still valid, otherwise false.
 */
bool
tempo_utils::BytesIterator::isValid() const
{
    return m_offset < m_bytes.size();
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
    if (!valid_request(m_bytes.size(), m_offset, 1))
        return false;
    auto *data = m_bytes.data();
    u8 = data[m_offset];
    m_offset++;
    return true;
}
bool
tempo_utils::BytesIterator::nextS8(tu_int8 &s8)
{
    if (!valid_request(m_bytes.size(), m_offset, 1))
        return false;
    auto *data = (const tu_int8 *) m_bytes.data();
    s8 = data[m_offset];
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
    if (!valid_request(m_bytes.size(), m_offset, 2))
        return false;
    auto *data = m_bytes.data();
    u16 = boost::endian::load_big_u16(data + m_offset);
    m_offset += 2;
    return true;
}

/**
 * If the iterator is valid and has at least 2 bytes available to be consumed then store the value at the
 * cursor in `u16` and move the cursor forward by 2 bytes. The data is expected to contain a little-endian
 * uint16.
 *
 * @param u16 The location to receive the next uint16.
 * @return true if the iteration was successful, otherwise false.
 */
bool
tempo_utils::BytesIterator::nextU16LE(tu_uint16 &u16)
{
    if (!valid_request(m_bytes.size(), m_offset, 2))
        return false;
    auto *data = m_bytes.data();
    u16 = boost::endian::load_little_u16(data + m_offset);
    m_offset += 2;
    return true;
}

bool
tempo_utils::BytesIterator::nextS16(tu_int16 &s16)
{
    if (!valid_request(m_bytes.size(), m_offset, 2))
        return false;
    auto *data = m_bytes.data();
    s16 = boost::endian::load_big_s16(data + m_offset);
    m_offset += 2;
    return true;
}

bool
tempo_utils::BytesIterator::nextS16LE(tu_int16 &s16)
{
    if (!valid_request(m_bytes.size(), m_offset, 2))
        return false;
    auto *data = m_bytes.data();
    s16 = boost::endian::load_little_s16(data + m_offset);
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
    if (!valid_request(m_bytes.size(), m_offset, 4))
        return false;
    auto *data = m_bytes.data();
    u32 = boost::endian::load_big_u32(data + m_offset);
    m_offset += 4;
    return true;
}

/**
 * If the iterator is valid and has at least 4 bytes available to be consumed then store the value at the
 * cursor in `u32` and move the cursor forward by 4 bytes. The data is expected to contain a little-endian
 * uint32.
 *
 * @param u32 The location to receive the next uint32.
 * @return true if the iteration was successful, otherwise false.
 */
bool
tempo_utils::BytesIterator::nextU32LE(tu_uint32 &u32)
{
    if (!valid_request(m_bytes.size(), m_offset, 4))
        return false;
    auto *data = m_bytes.data();
    u32 = boost::endian::load_little_u32(data + m_offset);
    m_offset += 4;
    return true;
}

bool
tempo_utils::BytesIterator::nextS32(tu_int32 &s32)
{
    if (!valid_request(m_bytes.size(), m_offset, 4))
        return false;
    auto *data = m_bytes.data();
    s32 = boost::endian::load_big_s32(data + m_offset);
    m_offset += 4;
    return true;
}

bool
tempo_utils::BytesIterator::nextS32LE(tu_int32 &s32)
{
    if (!valid_request(m_bytes.size(), m_offset, 4))
        return false;
    auto *data = m_bytes.data();
    s32 = boost::endian::load_little_s32(data + m_offset);
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
    if (!valid_request(m_bytes.size(), m_offset, 8))
        return false;
    auto *data = m_bytes.data();
    u64 = boost::endian::load_big_u64(data + m_offset);
    m_offset += 8;
    return true;
}

/**
 * Consume the next 8 bytes of the iterator as a little-endian ordered uint64 value. If there is not enough
 * data left to yield a value then the iterator is not modified.
 *
 * @param u64 The location to receive the next uint64.
 * @return true if the iteration was successful, otherwise false.
 */
bool
tempo_utils::BytesIterator::nextU64LE(tu_uint64 &u64)
{
    if (!valid_request(m_bytes.size(), m_offset, 8))
        return false;
    auto *data = m_bytes.data();
    u64 = boost::endian::load_little_u64(data + m_offset);
    m_offset += 8;
    return true;
}

bool
tempo_utils::BytesIterator::nextS64(tu_int64 &s64)
{
    if (!valid_request(m_bytes.size(), m_offset, 8))
        return false;
    auto *data = m_bytes.data();
    s64 = boost::endian::load_big_s64(data + m_offset);
    m_offset += 8;
    return true;
}

bool
tempo_utils::BytesIterator::nextS64LE(tu_int64 &s64)
{
    if (!valid_request(m_bytes.size(), m_offset, 8))
        return false;
    auto *data = m_bytes.data();
    s64 = boost::endian::load_little_s64(data + m_offset);
    m_offset += 8;
    return true;
}

bool
tempo_utils::BytesIterator::nextF32(float &f32)
{
    if (!valid_request(m_bytes.size(), m_offset, 4))
        return false;
    auto *data = m_bytes.data();
    boost::endian::big_float32_buf_t buf;
    memcpy(&buf, data, sizeof(buf));
    f32 = buf.value();
    m_offset += 4;
    return true;
}

bool
tempo_utils::BytesIterator::nextF32LE(float &f32)
{
    if (!valid_request(m_bytes.size(), m_offset, 4))
        return false;
    auto *data = m_bytes.data();
    boost::endian::little_float32_buf_t buf;
    memcpy(&buf, data, sizeof(buf));
    f32 = buf.value();
    m_offset += 4;
    return true;
}

bool
tempo_utils::BytesIterator::nextF64(double &f64)
{
    if (!valid_request(m_bytes.size(), m_offset, 8))
        return false;
    auto *data = m_bytes.data();
    boost::endian::big_float64_buf_t buf;
    memcpy(&buf, data, sizeof(buf));
    f64 = buf.value();
    m_offset += 8;
    return true;
}

bool
tempo_utils::BytesIterator::nextF64LE(double &f64)
{
    if (!valid_request(m_bytes.size(), m_offset, 8))
        return false;
    auto *data = m_bytes.data();
    boost::endian::little_float64_buf_t buf;
    memcpy(&buf, data, sizeof(buf));
    f64 = buf.value();
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
tempo_utils::BytesIterator::nextSlice(std::span<const tu_uint8> &slice, tu_uint32 count)
{
    if (!valid_request(m_bytes.size(), m_offset, count))
        return false;
    slice = m_bytes.subspan(m_offset, count);
    m_offset += count;
    return true;
}

size_t
tempo_utils::BytesIterator::bytesConsumed() const
{
    return m_offset;
}

size_t
tempo_utils::BytesIterator::bytesLeft() const
{
    return m_bytes.size() - m_offset;
}
