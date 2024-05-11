
#include <tempo_utils/log_message.h>
#include <tempo_utils/slice.h>

tempo_utils::Slice::Slice()
    : m_pos(0),
      m_count(0)
{
}

tempo_utils::Slice::Slice(std::shared_ptr<const ImmutableBytes> bytes)
    : m_bytes(std::move(bytes))
{
    TU_ASSERT (m_bytes != nullptr);
    m_pos = 0;
    m_count = m_bytes->getSize();
}

tempo_utils::Slice::Slice(std::shared_ptr<const ImmutableBytes> bytes, tu_uint32 pos)
    : m_bytes(std::move(bytes))
{
    TU_ASSERT (m_bytes != nullptr);
    auto size = m_bytes->getSize();
    m_pos = pos < size? pos : size;
    m_count = size - m_pos;
}

tempo_utils::Slice::Slice(std::shared_ptr<const ImmutableBytes> bytes, tu_uint32 pos, tu_uint32 count)
    : m_bytes(std::move(bytes))
{
    TU_ASSERT (m_bytes != nullptr);
    auto size = m_bytes->getSize();
    m_pos = pos < size? pos : size;
    m_count = m_pos + count <= size? count : size - m_pos;
}

tempo_utils::Slice::Slice(const Slice &other)
    : m_bytes(other.m_bytes),
      m_pos(other.m_pos),
      m_count(other.m_count)
{
}

tempo_utils::Slice::Slice(Slice &&other) noexcept
{
    m_bytes.swap(other.m_bytes);
    m_pos = other.m_pos;
    m_count = other.m_count;
}

tempo_utils::Slice&
tempo_utils::Slice::operator=(const Slice &other)
{
    if (this != &other) {
        m_bytes = other.m_bytes;
        m_pos = other.m_pos;
        m_count = other.m_count;
    }
    return *this;
}

tempo_utils::Slice&
tempo_utils::Slice::operator=(Slice &&other) noexcept
{
    if (this != &other) {
        m_bytes.swap(other.m_bytes);
        m_pos = other.m_pos;
        m_count = other.m_count;
    }
    return *this;
}

bool
tempo_utils::Slice::isEmpty() const
{
    return m_count == 0;
}

std::span<const tu_uint8>
tempo_utils::Slice::sliceView() const
{
    if (isEmpty())
        return {};
    auto *data = m_bytes->getData() + m_pos;
    return std::span<const tu_uint8>(data, m_count);
}

tu_uint8
tempo_utils::Slice::getByte(tu_uint32 index) const
{
    if (isEmpty())
        return 0;
    if (m_count <= index)
        return 0;
    auto *data = m_bytes->getData() + m_pos;
    return data[index];
}

const tu_uint8 *
tempo_utils::Slice::getData() const
{
    return m_bytes->getData() + m_pos;
}

tu_uint32
tempo_utils::Slice::getSize() const
{
    return m_count;
}

tempo_utils::Slice
tempo_utils::Slice::slice(tu_uint32 pos, tu_uint32 count) const
{
    if (isEmpty())
        return {};
    return Slice(m_bytes, m_pos + pos, count);
}

std::shared_ptr<const tempo_utils::ImmutableBytes>
tempo_utils::Slice::toImmutableBytes() const
{
    return std::make_shared<const Slice>(m_bytes, m_pos, m_count);
}
