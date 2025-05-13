#ifndef TEMPO_UTILS_DIRECTORY_MAKER_H
#define TEMPO_UTILS_DIRECTORY_MAKER_H

#include <filesystem>

#include "status.h"

namespace tempo_utils {

    constexpr std::filesystem::perms kDefaultDirectoryMakerPerms =
        std::filesystem::perms::owner_all
        | std::filesystem::perms::group_read
        | std::filesystem::perms::group_exec
        | std::filesystem::perms::others_read
        | std::filesystem::perms::others_exec
        ;

    class DirectoryMaker {

    public:
        explicit DirectoryMaker(
            const std::filesystem::path &absolutePath,
            std::filesystem::perms perms = kDefaultDirectoryMakerPerms);
        DirectoryMaker(
            const std::filesystem::path &baseDir,
            const std::filesystem::path &relativePath,
            std::filesystem::perms perms = kDefaultDirectoryMakerPerms);
        DirectoryMaker(const DirectoryMaker &other) = delete;

        bool isValid() const;
        Status getStatus() const;

        std::filesystem::path getAbsolutePath() const;

    private:
        std::filesystem::path m_absolutePath;
        Status m_status;
    };
}

#endif // TEMPO_UTILS_DIRECTORY_MAKER_H
