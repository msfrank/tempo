#ifndef TEMPO_UTILS_FILE_APPENDER_H
#define TEMPO_UTILS_FILE_APPENDER_H

#include <filesystem>
#include <span>
#include <vector>

#include "bytes_appender.h"
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
        Status appendS8(tu_int8 s8);
        Status appendU16(tu_uint16 u16);
        Status appendU16LE(tu_uint16 u16);
        Status appendS16(tu_int16 s16);
        Status appendS16LE(tu_int16 s16);
        Status appendU32(tu_uint32 u32);
        Status appendU32LE(tu_uint32 u32);
        Status appendS32(tu_int32 s32);
        Status appendS32LE(tu_int32 s32);
        Status appendU64(tu_uint64 u64);
        Status appendU64LE(tu_uint64 u64);
        Status appendS64(tu_int64 s64);
        Status appendS64LE(tu_int64 s64);
        Status appendF32(float f32);
        Status appendF32LE(float f32);
        Status appendF64(double f64);
        Status appendF64LE(double f64);
        Status appendBytes(const std::shared_ptr<const ImmutableBytes> &bytes);
        Status appendBytes(std::span<const tu_uint8> bytes);
        Status appendBytes(std::string_view str);

        Status finish();

    private:
        std::filesystem::path m_absolutePath;
        FileAppenderMode m_mode;
        Status m_status;
        std::unique_ptr<BytesAppender> m_buf;
        int m_fd;
    };
}

#endif // TEMPO_UTILS_FILE_APPENDER_H
