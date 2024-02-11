#ifndef TEMPO_UTILS_TMPDIR_MAKER_H
#define TEMPO_UTILS_TMPDIR_MAKER_H

#include <filesystem>

#include "status.h"

namespace tempo_utils {

    class TempdirMaker {

    public:
        TempdirMaker(std::string_view tempname);
        TempdirMaker(const std::filesystem::path &base, std::string_view tempname);
        TempdirMaker(const TempdirMaker &other) = delete;

        bool isValid() const;
        tempo_utils::Status getStatus() const;
        std::filesystem::path getTempdir() const;

    private:
        tempo_utils::Status m_status;
        std::filesystem::path m_tempdir;
    };
}

#endif // TEMPO_UTILS_TMPDIR_MAKER_H
