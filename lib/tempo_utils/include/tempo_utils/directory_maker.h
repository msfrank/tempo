#ifndef DIRECTORY_MAKER_H
#define DIRECTORY_MAKER_H

#include <filesystem>

#include "status.h"

namespace tempo_utils {

    class DirectoryMaker {

    public:
        explicit DirectoryMaker(const std::filesystem::path &absolutePath);
        DirectoryMaker(const std::filesystem::path &baseDir, const std::filesystem::path &relativePath);
        DirectoryMaker(const DirectoryMaker &other) = delete;

        bool isValid() const;
        tempo_utils::Status getStatus() const;

        std::filesystem::path getAbsolutePath() const;

    private:
        std::filesystem::path m_absolutePath;
        tempo_utils::Status m_status;
    };
}

#endif // DIRECTORY_MAKER_H
