#ifndef TEMPO_UTILS_LIBRARY_LOADER_H
#define TEMPO_UTILS_LIBRARY_LOADER_H

#include <filesystem>

#include <tempo_utils/status.h>

namespace tempo_utils {

    class LibraryLoader {

    public:
        LibraryLoader(const std::filesystem::path &path, const char *symbolName);
        LibraryLoader(const LibraryLoader &other) = delete;

        bool isValid() const;
        Status getStatus() const;

        std::filesystem::path getAbsolutePath() const;
        void *symbolPointer() const;

    private:
        std::filesystem::path m_absolutePath;
        void *m_symbol;
        Status m_status;
    };
}

#endif // TEMPO_UTILS_LIBRARY_LOADER_H