#ifndef TEMPO_UTILS_FILE_APPENDER_H
#define TEMPO_UTILS_FILE_APPENDER_H

#include <filesystem>
#include <span>
#include <vector>

#include "immutable_bytes.h"
#include "integer_types.h"
#include "status.h"

namespace tempo_utils {

    enum class FileAppenderMode {
        APPEND,
        CREATE_OR_APPEND,
        CREATE_OR_OVERWRITE,
        CREATE_ONLY,
    };

    constexpr std::filesystem::perms kDefaultFileAppenderPerms =
        std::filesystem::perms::owner_read
        | std::filesystem::perms::owner_write
        ;

    class FileAppender {

    public:
        explicit FileAppender(const std::filesystem::path &path);
        FileAppender(
            const std::filesystem::path &path,
            FileAppenderMode mode,
            std::filesystem::perms perms = kDefaultFileAppenderPerms);
        FileAppender(const FileAppender &other) = delete;
        ~FileAppender();

        bool isValid() const;
        Status getStatus() const;
        std::filesystem::path getAbsolutePath() const;
        FileAppenderMode getMode();

        Status appendU8(tu_uint8 u8);
        Status appendU16(tu_uint16 u16);
        Status appendU32(tu_uint32 u32);
        Status appendBytes(std::shared_ptr<ImmutableBytes> bytes);
        Status appendBytes(std::span<const tu_uint8> bytes);
        Status appendBytes(std::string_view str);

        Status finish();

    private:
        std::filesystem::path m_absolutePath;
        FileAppenderMode m_mode;
        Status m_status;
        int m_fd;
    };
}

#endif // TEMPO_UTILS_FILE_APPENDER_H
