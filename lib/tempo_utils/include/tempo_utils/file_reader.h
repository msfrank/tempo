#ifndef FILE_READER_H
#define FILE_READER_H

#include <filesystem>

#include "immutable_bytes.h"
#include "status.h"

namespace tempo_utils {

    class FileReader {

    public:
        FileReader(const std::filesystem::path &path);
        FileReader(const FileReader &other) = delete;

        bool isValid() const;
        tempo_utils::Status getStatus() const;

        std::filesystem::path getAbsolutePath() const;
        std::shared_ptr<const ImmutableBytes> getBytes() const;

    private:
        std::filesystem::path m_absolutePath;
        std::shared_ptr<const ImmutableBytes> m_bytes;
        tempo_utils::Status m_status;
    };
}

#endif // FILE_READER_H
