#ifndef TEMPO_UTILS_FILE_UTILITIES_H
#define TEMPO_UTILS_FILE_UTILITIES_H

#include <filesystem>

#include <absl/strings/string_view.h>

namespace tempo_utils {

    bool is_valid_tempname(std::string_view tempname);

    std::string generate_name(std::string_view tempname);

};


#endif // TEMPO_UTILS_FILE_UTILITIES_H
