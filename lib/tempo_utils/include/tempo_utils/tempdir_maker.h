#ifndef TEMPO_UTILS_TEMPDIR_MAKER_H
#define TEMPO_UTILS_TEMPDIR_MAKER_H

#include <filesystem>

#include "status.h"

namespace tempo_utils {

    class TempdirMaker {

    public:
        explicit TempdirMaker(std::string_view tempname);
        TempdirMaker(const std::filesystem::path &base, std::string_view tempname);
        TempdirMaker(const TempdirMaker &other) = delete;

        bool isValid() const;
        Status getStatus() const;

        std::filesystem::path getTempdir() const;

    private:
        Status m_status;
        std::filesystem::path m_tempdir;
    };
}

#endif // TEMPO_UTILS_TEMPDIR_MAKER_H
