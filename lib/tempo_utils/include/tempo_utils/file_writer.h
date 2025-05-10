#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include <filesystem>
#include <span>

#include "status.h"

namespace tempo_utils {

    enum class FileWriterMode {
        APPEND,
        CREATE_OR_OVERWRITE,
        CREATE_ONLY,
        OVERWRITE_ONLY,
    };

    class FileWriter {

    public:
        FileWriter(const std::filesystem::path &path, std::span<const tu_uint8> bytes, FileWriterMode mode);
        FileWriter(const std::filesystem::path &path, std::string_view str, FileWriterMode mode);
        FileWriter(
            const std::filesystem::path &path,
            std::span<const tu_uint8> bytes,
            FileWriterMode mode,
            std::filesystem::perms perms);
        FileWriter(
            const std::filesystem::path &path,
            std::string_view str,
            FileWriterMode mode,
            std::filesystem::perms perms);
        FileWriter(const FileWriter &other) = delete;

        bool isValid() const;
        tempo_utils::Status getStatus() const;
        std::filesystem::path getAbsolutePath() const;
        FileWriterMode getMode();

    private:
        std::filesystem::path m_absolutePath;
        FileWriterMode m_mode;
        Status m_status;
    };
}

#endif // FILE_WRITER_H
