
#include <tempo_utils/bytes_appender.h>
#include <tempo_utils/big_endian.h>

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
    TU_ASSERT (m_bytes != nullptr);
    m_bytes->push_back((char) u8);
}

void
tempo_utils::BytesAppender::appendU16(tu_uint16 u16)
{
    TU_ASSERT (m_bytes != nullptr);
    auto size = m_bytes->size();
    m_bytes->resize(size + 2);
    auto *ptr = ((tu_uint8 *) m_bytes->data()) + size;
    write_u16(u16, ptr);
}

void
tempo_utils::BytesAppender::appendU32(tu_uint32 u32)
{
    TU_ASSERT (m_bytes != nullptr);
    auto size = m_bytes->size();
    m_bytes->resize(size + 4);
    auto *ptr = ((tu_uint8 *) m_bytes->data()) + size;
    write_u32(u32, ptr);
}

void
tempo_utils::BytesAppender::appendBytes(std::span<const tu_uint8> bytes)
{
    TU_ASSERT (m_bytes != nullptr);
    m_bytes->insert(m_bytes->end(), bytes.begin(), bytes.end());
}

void
tempo_utils::BytesAppender::appendBytes(std::string_view str)
{
    TU_ASSERT (m_bytes != nullptr);
    m_bytes->insert(m_bytes->end(), str.begin(), str.end());
}

tu_uint32
tempo_utils::BytesAppender::getSize() const
{
    TU_ASSERT (m_bytes != nullptr);
    return m_bytes->size();
}

std::vector<tu_uint8>::const_iterator
tempo_utils::BytesAppender::bytesBegin() const
{
    TU_ASSERT (m_bytes != nullptr);
    return m_bytes->cbegin();
}

std::vector<tu_uint8>::const_iterator
tempo_utils::BytesAppender::bytesEnd() const
{
    TU_ASSERT (m_bytes != nullptr);
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