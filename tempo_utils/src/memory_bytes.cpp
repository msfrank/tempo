
#include <tempo_utils/memory_bytes.h>

tempo_utils::MemoryBytes::MemoryBytes(std::vector<tu_uint8> &&bytes)
    : m_bytes(std::move(bytes))
{
}

const tu_uint8 *
tempo_utils::MemoryBytes::getData() const
{
    return m_bytes.data();
}

tu_uint32
tempo_utils::MemoryBytes::getSize() const
{
    return m_bytes.size();
}

std::span<const tu_uint8>
tempo_utils::MemoryBytes::getSpan() const
{
    return std::span<const tu_uint8>(m_bytes.data(), m_bytes.size());
}

tempo_utils::Slice
tempo_utils::MemoryBytes::toSlice() const
{
    return Slice(shared_from_this());
}

std::shared_ptr<tempo_utils::MemoryBytes>
tempo_utils::MemoryBytes::create(std::vector<tu_uint8> &&bytes)
{
    return std::shared_ptr<MemoryBytes>(new MemoryBytes(std::move(bytes)));
}

std::shared_ptr<tempo_utils::MemoryBytes>
tempo_utils::MemoryBytes::copy(std::span<const tu_uint8> bytes)
{
    if (bytes.empty())
        return std::shared_ptr<MemoryBytes>(new MemoryBytes());
    std::vector<tu_uint8> v(bytes.begin(), bytes.end());
    return std::shared_ptr<MemoryBytes>(new MemoryBytes(std::move(v)));
}

std::shared_ptr<tempo_utils::MemoryBytes>
tempo_utils::MemoryBytes::copy(std::initializer_list<tu_uint8> init)
{
    std::vector<tu_uint8> bytes(init.begin(), init.end());
    return std::shared_ptr<MemoryBytes>(new MemoryBytes(std::move(bytes)));
}

std::shared_ptr<tempo_utils::MemoryBytes>
tempo_utils::MemoryBytes::copy(std::string_view str)
{
    std::vector<tu_uint8> bytes(str.begin(), str.end());
    return std::shared_ptr<MemoryBytes>(new MemoryBytes(std::move(bytes)));
}
