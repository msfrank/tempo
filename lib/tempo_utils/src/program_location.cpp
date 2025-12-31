
#include <boost/dll.hpp>

#include <tempo_utils/program_location.h>

#include "tempo_utils/file_result.h"

/**
 * Return the path of the currently executing program.
 *
 * @return `tempo_utils::Result` containing the path to the currently executing program, or
 *   `tempo_utils::Status` if the path cannot be determined.
 */
tempo_utils::Result<std::filesystem::path>
tempo_utils::get_program_path()
{
    boost::dll::fs::error_code ec;
    auto path = boost::dll::program_location(ec);
    if (ec)
        return FileStatus::forCondition(FileCondition::kFileNotFound, ec.message());
    return std::filesystem::path{path.c_str()};
}

/**
 * Return the path to the directory containing the currently executing program.
 *
 * @return `tempo_utils::Result` containing the path to the directory containing the currently
 *   executing program, or `tempo_utils::Status` if the path cannot be determined.
 */
tempo_utils::Result<std::filesystem::path>
tempo_utils::get_program_directory()
{
    std::filesystem::path exe;
    TU_ASSIGN_OR_RETURN (exe, get_program_path());
    return exe.parent_path();
}