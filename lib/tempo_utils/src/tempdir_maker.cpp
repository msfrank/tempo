
#include <tempo_utils/file_result.h>
#include <tempo_utils/file_utilities.h>
#include <tempo_utils/posix_result.h>
#include <tempo_utils/tempdir_maker.h>

#define NUM_GENERATE_TRIES      3

#include <cerrno>
#include <sys/stat.h>

static tempo_utils::Status
posix_create_tempdir(
    const std::filesystem::path &base,
    std::string_view tempname,
    std::filesystem::path &tempdir)
{
    if (!tempo_utils::is_valid_tempname(tempname))
        return tempo_utils::FileStatus::forCondition(
            tempo_utils::FileCondition::kInvalidTemplate,
            "invalid template name '{}'", tempname);

    for (int attempt = 0; attempt < NUM_GENERATE_TRIES; attempt++) {
        auto filename = tempo_utils::generate_name(tempname);
        if (filename.empty())
            return tempo_utils::FileStatus::forCondition(
                tempo_utils::FileCondition::kSystemInvariant,
                "failed to generate filename from template '{}'", tempname);

        tempdir = base / filename;
        if (mkdir(tempdir.c_str(), 0700) == 0)
            return {};

        switch (errno) {
            case EEXIST:
                break;
            case EACCES:
                return tempo_utils::FileStatus::forCondition(
                    tempo_utils::FileCondition::kAccessDenied,
                    "access denied for {}", tempdir.string());
            case ENOENT:
                return tempo_utils::FileStatus::forCondition(
                    tempo_utils::FileCondition::kParentDirectoryNotFound,
                    "parent directory {} not found", base.string());
            default:
                return tempo_utils::PosixStatus::fromError(errno);
        }
    }

    // exhausted retries
    return tempo_utils::FileStatus::forCondition(
        tempo_utils::FileCondition::kSystemInvariant,
        "failed to generate unique directory");
}

tempo_utils::TempdirMaker::TempdirMaker(std::string_view tempname)
    : TempdirMaker(std::filesystem::temp_directory_path(), tempname)
{
}

tempo_utils::TempdirMaker::TempdirMaker(const std::filesystem::path &base, std::string_view tempname)
{
    m_status = posix_create_tempdir(base, tempname, m_tempdir);
}

bool
tempo_utils::TempdirMaker::isValid() const
{
    return m_status.isOk();
}

tempo_utils::Status
tempo_utils::TempdirMaker::getStatus() const
{
    return m_status;
}

std::filesystem::path
tempo_utils::TempdirMaker::getTempdir() const
{
    return m_tempdir;
}
