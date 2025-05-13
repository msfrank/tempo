#ifndef TEMPO_UTILS_TEMPFILE_MAKER_H
#define TEMPO_UTILS_TEMPFILE_MAKER_H

#include <filesystem>
#include <span>

#include "status.h"

namespace tempo_utils {

    class TempfileMaker {

    public:
        explicit TempfileMaker(std::string_view tempname, std::shared_ptr<ImmutableBytes> bytes = {});
        explicit TempfileMaker(std::string_view tempname, std::span<const tu_uint8> bytes = {});
        explicit TempfileMaker(std::string_view tempname, std::string_view str = {});
        TempfileMaker(
            const std::filesystem::path &base,
            std::string_view tempname,
            std::shared_ptr<ImmutableBytes> bytes = {});
        TempfileMaker(
            const std::filesystem::path &base,
            std::string_view tempname,
            std::span<const tu_uint8> bytes = {});
        TempfileMaker(
            const std::filesystem::path &base,
            std::string_view tempname,
            std::string_view str = {});
        TempfileMaker(const TempfileMaker &other) = delete;

        bool isValid() const;
        Status getStatus() const;
        std::filesystem::path getTempfile() const;

    private:
        Status m_status;
        std::filesystem::path m_tempfile;
    };
}

#endif // TEMPO_UTILS_TEMPFILE_MAKER_H
