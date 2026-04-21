
#include <boost/endian.hpp>

#include <tempo_utils/bytes_appender.h>
#include <tempo_utils/log_message.h>

tempo_utils::BytesAppender::BytesAppender(int initialCapacity)
    : m_bytes(std::make_unique<std::vector<tu_uint8>>())
{
    if (initialCapacity > 0) {
        m_bytes->reserve(initialCapacity);
    }
}

void
tempo_utils::BytesAppender::appendU8(tu_uint8 u8)
{
    TU_NOTNULL (m_bytes);
    m_bytes->push_back(u8);
}

void
tempo_utils::BytesAppender::appendS8(tu_int8 s8)
{
    TU_NOTNULL (m_bytes);
    auto size = m_bytes->size();
    m_bytes->resize(size + 1);
    auto *ptr = m_bytes->data() + size;
    memcpy(ptr, &s8, 1);
}

void
tempo_utils::BytesAppender::appendU16(tu_uint16 u16)
{
    TU_NOTNULL (m_bytes);
    boost::endian::big_uint16_buf_t buf{u16};
    size_t count = sizeof(buf);
    auto size = m_bytes->size();
    m_bytes->resize(size + count);
    auto *ptr = m_bytes->data() + size;
    memcpy(ptr, &buf, count);
}

void
tempo_utils::BytesAppender::appendU16LE(tu_uint16 u16)
{
    TU_NOTNULL (m_bytes);
    boost::endian::little_uint16_buf_t buf{u16};
    size_t count = sizeof(buf);
    auto size = m_bytes->size();
    m_bytes->resize(size + count);
    auto *ptr = m_bytes->data() + size;
    memcpy(ptr, &buf, count);
}

void
tempo_utils::BytesAppender::appendS16(tu_int16 s16)
{
    TU_NOTNULL (m_bytes);
    boost::endian::big_int16_buf_t buf{s16};
    size_t count = sizeof(buf);
    auto size = m_bytes->size();
    m_bytes->resize(size + count);
    auto *ptr = m_bytes->data() + size;
    memcpy(ptr, &buf, count);
}

void
tempo_utils::BytesAppender::appendS16LE(tu_int16 s16)
{
    TU_NOTNULL (m_bytes);
    boost::endian::little_int16_buf_t buf{s16};
    size_t count = sizeof(buf);
    auto size = m_bytes->size();
    m_bytes->resize(size + count);
    auto *ptr = m_bytes->data() + size;
    memcpy(ptr, &buf, count);
}

void
tempo_utils::BytesAppender::appendU32(tu_uint32 u32)
{
    TU_NOTNULL (m_bytes);
    boost::endian::big_uint32_buf_t buf{u32};
    size_t count = sizeof(buf);
    auto size = m_bytes->size();
    m_bytes->resize(size + count);
    auto *ptr = m_bytes->data() + size;
    memcpy(ptr, &buf, count);
}

void
tempo_utils::BytesAppender::appendU32LE(tu_uint32 u32)
{
    TU_NOTNULL (m_bytes);
    boost::endian::little_uint32_buf_t buf{u32};
    size_t count = sizeof(buf);
    auto size = m_bytes->size();
    m_bytes->resize(size + count);
    auto *ptr = m_bytes->data() + size;
    memcpy(ptr, &buf, count);
}

void
tempo_utils::BytesAppender::appendS32(tu_int32 s32)
{
    TU_NOTNULL (m_bytes);
    boost::endian::big_int32_buf_t buf{s32};
    size_t count = sizeof(buf);
    auto size = m_bytes->size();
    m_bytes->resize(size + count);
    auto *ptr = m_bytes->data() + size;
    memcpy(ptr, &buf, count);
}

void
tempo_utils::BytesAppender::appendS32LE(tu_int32 s32)
{
    TU_NOTNULL (m_bytes);
    boost::endian::little_int32_buf_t buf{s32};
    size_t count = sizeof(buf);
    auto size = m_bytes->size();
    m_bytes->resize(size + count);
    auto *ptr = m_bytes->data() + size;
    memcpy(ptr, &buf, count);
}

void
tempo_utils::BytesAppender::appendU64(tu_uint64 u64)
{
    TU_NOTNULL (m_bytes);
    boost::endian::big_uint64_buf_t buf{u64};
    size_t count = sizeof(buf);
    auto size = m_bytes->size();
    m_bytes->resize(size + count);
    auto *ptr = m_bytes->data() + size;
    memcpy(ptr, &buf, count);
}

void
tempo_utils::BytesAppender::appendU64LE(tu_uint64 u64)
{
    TU_NOTNULL (m_bytes);
    boost::endian::little_uint64_buf_t buf{u64};
    size_t count = sizeof(buf);
    auto size = m_bytes->size();
    m_bytes->resize(size + count);
    auto *ptr = m_bytes->data() + size;
    memcpy(ptr, &buf, count);
}

void
tempo_utils::BytesAppender::appendS64(tu_int64 s64)
{
    TU_NOTNULL (m_bytes);
    boost::endian::big_int64_buf_t buf{s64};
    size_t count = sizeof(buf);
    auto size = m_bytes->size();
    m_bytes->resize(size + count);
    auto *ptr = m_bytes->data() + size;
    memcpy(ptr, &buf, count);
}

void
tempo_utils::BytesAppender::appendS64LE(tu_int64 s64)
{
    TU_NOTNULL (m_bytes);
    boost::endian::little_int64_buf_t buf{s64};
    size_t count = sizeof(buf);
    auto size = m_bytes->size();
    m_bytes->resize(size + count);
    auto *ptr = m_bytes->data() + size;
    memcpy(ptr, &buf, count);
}

void
tempo_utils::BytesAppender::appendF32(float f32)
{
    TU_NOTNULL (m_bytes);
    boost::endian::big_float32_buf_t buf{f32};
    size_t count = sizeof(buf);
    auto size = m_bytes->size();
    m_bytes->resize(size + count);
    auto *ptr = m_bytes->data() + size;
    memcpy(ptr, &buf, count);
}

void
tempo_utils::BytesAppender::appendF32LE(float f32)
{
    TU_NOTNULL (m_bytes);
    boost::endian::little_float32_buf_t buf{f32};
    size_t count = sizeof(buf);
    auto size = m_bytes->size();
    m_bytes->resize(size + count);
    auto *ptr = m_bytes->data() + size;
    memcpy(ptr, &buf, count);
}

void
tempo_utils::BytesAppender::appendF64(double f64)
{
    TU_NOTNULL (m_bytes);
    boost::endian::big_float64_buf_t buf{f64};
    size_t count = sizeof(buf);
    auto size = m_bytes->size();
    m_bytes->resize(size + count);
    auto *ptr = m_bytes->data() + size;
    memcpy(ptr, &buf, count);
}

void
tempo_utils::BytesAppender::appendF64LE(double f64)
{
    TU_NOTNULL (m_bytes);
    boost::endian::little_float64_buf_t buf{f64};
    size_t count = sizeof(buf);
    auto size = m_bytes->size();
    m_bytes->resize(size + count);
    auto *ptr = m_bytes->data() + size;
    memcpy(ptr, &buf, count);
}

void
tempo_utils::BytesAppender::appendBytes(std::span<const tu_uint8> bytes)
{
    TU_NOTNULL (m_bytes);
    m_bytes->insert(m_bytes->end(), bytes.begin(), bytes.end());
}

void
tempo_utils::BytesAppender::appendBytes(std::string_view str)
{
    TU_NOTNULL (m_bytes);
    m_bytes->insert(m_bytes->end(), str.begin(), str.end());
}

void
tempo_utils::BytesAppender::appendBytes(const BytesAppender &appender)
{
    m_bytes->insert(m_bytes->end(), appender.bytesBegin(), appender.bytesEnd());
}

const tu_uint8 *
tempo_utils::BytesAppender::getData() const
{
    return m_bytes->data();
}

tu_uint32
tempo_utils::BytesAppender::getSize() const
{
    TU_NOTNULL (m_bytes);
    return m_bytes->size();
}

std::vector<tu_uint8>::const_iterator
tempo_utils::BytesAppender::bytesBegin() const
{
    TU_NOTNULL (m_bytes);
    return m_bytes->cbegin();
}

std::vector<tu_uint8>::const_iterator
tempo_utils::BytesAppender::bytesEnd() const
{
    TU_NOTNULL (m_bytes);
    return m_bytes->cend();
}

std::shared_ptr<const tempo_utils::MemoryBytes>
tempo_utils::BytesAppender::finish()
{
    if (m_bytes == nullptr)
        return {};
    auto bytes = MemoryBytes::create(std::move(*m_bytes));
    m_bytes.reset();
    return bytes;
}