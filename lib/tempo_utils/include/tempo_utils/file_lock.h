#ifndef TEMPO_UTILS_FILE_LOCK_H
#define TEMPO_UTILS_FILE_LOCK_H

#include <filesystem>

#include "file_writer.h"
#include "status.h"

namespace tempo_utils {

    constexpr std::filesystem::perms kDefaultFileLockPerms =
        std::filesystem::perms::owner_read
        | std::filesystem::perms::owner_write
        ;

    class FileLock {
    public:
        explicit FileLock(
            const std::filesystem::path &path,
            int lockTimeoutInMs = -1,
            std::filesystem::perms perms = kDefaultFileLockPerms);
        FileLock(
            const std::filesystem::path &path,
            std::string_view str,
            int lockTimeoutInMs = -1,
            std::filesystem::perms perms = kDefaultFileLockPerms);
        FileLock(const FileLock &other) = delete;
        ~FileLock();

        bool isValid() const;
        Status getStatus() const;

        bool isLocked() const;
        Status unlock();

    private:
        std::unique_ptr<FileWriter> m_writer;
        Status m_status;
    };
}

#endif // TEMPO_UTILS_FILE_LOCK_H