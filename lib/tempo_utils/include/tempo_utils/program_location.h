#ifndef TEMPO_UTILS_PROGRAM_LOCATION_H
#define TEMPO_UTILS_PROGRAM_LOCATION_H

#include <filesystem>

#include "result.h"

namespace tempo_utils {

    Result<std::filesystem::path> get_program_path();

    Result<std::filesystem::path> get_program_directory();
}

#endif // TEMPO_UTILS_PROGRAM_LOCATION_H