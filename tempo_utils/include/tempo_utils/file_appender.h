#ifndef TEMPO_UTILS_FILE_APPENDER_H
#define TEMPO_UTILS_FILE_APPENDER_H

#include <filesystem>
#include <span>
#include <vector>

#include "integer_types.h"
#include "status.h"

namespace tempo_utils {

    enum class FileAppenderMode {
        APPEND,
        CREATE_OR_APPEND,
        CREATE_OR_OVERWRITE,
        CREATE_ONLY,
    };

    class FileAppender {

    public:
        FileAppender(const std::filesystem::path &path);
        FileAppender(const std::filesystem::path &path, FileAppenderMode mode);
        FileAppender(const std::filesystem::path &path, FileAppenderMode mode, std::filesystem::perms perms);
        FileAppender(const FileAppender &other) = delete;
        ~FileAppender();

        bool isValid() const;
        tempo_utils::Status getStatus() const;
        std::filesystem::path getAbsolutePath() const;
        FileAppenderMode getMode();

        tempo_utils::Status appendU8(tu_uint8 u8);
        tempo_utils::Status appendU16(tu_uint16 u16);
        tempo_utils::Status appendU32(tu_uint32 u32);
        tempo_utils::Status appendBytes(std::span<const tu_uint8> bytes);
        tempo_utils::Status appendBytes(std::string_view str);

        tempo_utils::Status finish();

    private:
        std::filesystem::path m_absolutePath;
        FileAppenderMode m_mode;
        tempo_utils::Status m_status;
        int m_fd;
    };
}

#endif // TEMPO_UTILS_FILE_APPENDER_H
