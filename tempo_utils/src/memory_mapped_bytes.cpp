
#include <boost/interprocess/managed_mapped_file.hpp>

#include <tempo_utils/file_result.h>
#include <tempo_utils/log_message.h>
#include <tempo_utils/memory_mapped_bytes.h>

struct tempo_utils::MemoryMappedBytes::Priv {
    boost::interprocess::mapped_region mappedRegion;
};

tempo_utils::MemoryMappedBytes::MemoryMappedBytes(std::unique_ptr<Priv> &&priv)
    : m_priv(std::move(priv))
{
    TU_ASSERT (m_priv != nullptr);
}

const tu_uint8 *
tempo_utils::MemoryMappedBytes::getData() const
{
    if (m_priv == nullptr)
        return nullptr;
    return (const uint8_t *) m_priv->mappedRegion.get_address();
}

tu_uint32
tempo_utils::MemoryMappedBytes::getSize() const
{
    if (m_priv == nullptr)
        return 0;
    return m_priv->mappedRegion.get_size();
}

std::span<const tu_uint8>
tempo_utils::MemoryMappedBytes::getSpan() const
{
    if (m_priv == nullptr)
        return {};
    return std::span<const tu_uint8>(getData(), getSize());
}

tempo_utils::Slice
tempo_utils::MemoryMappedBytes::toSlice() const
{
    return Slice(shared_from_this());
}

tempo_utils::Result<std::shared_ptr<tempo_utils::MemoryMappedBytes>>
tempo_utils::MemoryMappedBytes::open(const std::filesystem::path &path)
{
    try {
        boost::interprocess::file_mapping fileMapping(path.c_str(), boost::interprocess::read_only);
        boost::interprocess::mapped_region mappedRegion(fileMapping, boost::interprocess::read_only);
        auto priv = std::make_unique<Priv>();
        priv->mappedRegion = std::move(mappedRegion);
        return std::shared_ptr<MemoryMappedBytes>(new MemoryMappedBytes(std::move(priv)));
    } catch (boost::interprocess::interprocess_exception &ex) {
        switch (ex.get_error_code()) {
            case boost::interprocess::not_found_error:
                return FileStatus::forCondition(FileCondition::kFileNotFound,
                    "failed to mmap file {}: {}", path.string(), ex.what());
            case boost::interprocess::security_error:
                return FileStatus::forCondition(FileCondition::kAccessDenied,
                    "failed to mmap file {}: {}", path.string(), ex.what());
            default:
                return GenericStatus::forCondition(GenericCondition::kInternalViolation,
                    "failed to mmap file {}: {}", path.string(), ex.what());
        }
    } catch (std::exception &ex) {
        return GenericStatus::forCondition(GenericCondition::kInternalViolation,
            "failed to mmap file {}: {}", path.string(), ex.what());
    }
}