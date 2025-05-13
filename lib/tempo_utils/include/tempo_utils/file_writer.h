#ifndef TEMPO_UTILS_FILE_WRITER_H
#define TEMPO_UTILS_FILE_WRITER_H

#include <filesystem>
#include <span>

#include "immutable_bytes.h"
#include "status.h"

namespace tempo_utils {

    enum class FileWriterMode {
        APPEND,
        CREATE_OR_OVERWRITE,
        CREATE_ONLY,
        OVERWRITE_ONLY,
    };

    constexpr std::filesystem::perms kDefaultFileWriterPerms =
        std::filesystem::perms::owner_read
        | std::filesystem::perms::owner_write
        ;

    class FileWriter {
    public:
        FileWriter(
            const std::filesystem::path &path,
            std::shared_ptr<ImmutableBytes> bytes,
            FileWriterMode mode,
            std::filesystem::perms perms = kDefaultFileWriterPerms);
        FileWriter(
            const std::filesystem::path &path,
            std::span<const tu_uint8> bytes,
            FileWriterMode mode,
            std::filesystem::perms perms = kDefaultFileWriterPerms);
        FileWriter(
            const std::filesystem::path &path,
            std::string_view str,
            FileWriterMode mode,
            std::filesystem::perms perms = kDefaultFileWriterPerms);
        FileWriter(const FileWriter &other) = delete;

        bool isValid() const;
        Status getStatus() const;
        std::filesystem::path getAbsolutePath() const;
        FileWriterMode getMode();

    private:
        std::filesystem::path m_absolutePath;
        FileWriterMode m_mode;
        Status m_status;
    };
}

#endif // TEMPO_UTILS_FILE_WRITER_H
