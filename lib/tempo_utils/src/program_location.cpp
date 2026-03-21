
#include <boost/dll.hpp>

#include <tempo_utils/file_result.h>
#include <tempo_utils/program_location.h>

/**
 * Return the path of the currently executing program. If the program was invoked via a symbolic
 * link then the link is resolved to the file path of the executable.
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
        return FileStatus::forCondition(FileCondition::kSystemInvariant, ec.message());
    std::filesystem::path programPath{path.c_str()};

    int count = 0;
    while (std::filesystem::is_symlink(programPath) && ++count < kMaxSymlinks + 1) {
        programPath = std::filesystem::read_symlink(programPath, ec);
        if (ec)
            return FileStatus::forCondition(FileCondition::kSystemInvariant, ec.message());
    }

    if (count <= kMaxSymlinks)
        return programPath;

    return FileStatus::forCondition(FileCondition::kSystemInvariant,
        "encountered too many symlinks trying to resolve {}", path.string());
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