#include <sys/stat.h>

#include <tempo_utils/directory_maker.h>
#include <tempo_utils/file_result.h>
#include <tempo_utils/posix_result.h>

static tempo_utils::Status
posix_create_directories(const std::filesystem::path &absolutePath, const std::filesystem::perms &perms)
{
    TU_ASSERT (!absolutePath.empty());

    // initialize curr, start iterator at the next element past the root-directory
    auto iterator = absolutePath.begin();
    std::filesystem::path curr = *iterator++;
    if (*iterator == "/")
        curr /= *iterator++;

    int ret;

    // for each subsequent non-empty element, create the directory if needed and chdir into the dir
    for (; iterator != absolutePath.end(); iterator++) {
        if (iterator->empty())                              // ignore an empty path part
            continue;
        const char *name = iterator->c_str();
        curr /= name;                                       // append part to the curr path
        const char *path = curr.c_str();
        struct stat buf;
        ret = stat(path, &buf);                             // check if curr path exists and is accessible
        if (ret < 0) {
            switch (errno) {
                case ENOENT:                                // curr path doesn't exist
                    break;
                case EACCES:                                // curr path is not accessible
                    return tempo_utils::FileStatus::forCondition(
                        tempo_utils::FileCondition::kAccessDenied,
                        "access denied for {}", path);
                default:                                    // any other error is considered an internal violation
                    return tempo_utils::PosixStatus::fromError(errno);
            }
            ret = mkdir(path, static_cast<mode_t>(perms));  // create missing directory
            if (ret < 0) {
                if (errno == EACCES)
                    return tempo_utils::FileStatus::forCondition(
                        tempo_utils::FileCondition::kAccessDenied,
                        "access denied for {}", path);
                return tempo_utils::PosixStatus::fromError(errno);
            }
        }
    }
    return {};
}

tempo_utils::DirectoryMaker::DirectoryMaker(
    const std::filesystem::path &absolutePath,
    std::filesystem::perms perms)
{
    if (absolutePath.empty()) {
        m_status = FileStatus::forCondition(FileCondition::kInvalidName, "missing path");
    } else {
        m_absolutePath = absolutePath;
        m_status = posix_create_directories(m_absolutePath, perms);
    }
}

tempo_utils::DirectoryMaker::DirectoryMaker(
    const std::filesystem::path &baseDir,
    const std::filesystem::path &relativePath,
    std::filesystem::perms perms)
{
    if (!exists(baseDir)) {
        m_status = FileStatus::forCondition(FileCondition::kMissingBaseDirectory,
            "missing base directory {}", baseDir.string());
    } else if (!is_directory(baseDir)) {
        m_status = FileStatus::forCondition(FileCondition::kMissingBaseDirectory,
            "invalid base directory {}", baseDir.string());
    } else if (relativePath.empty()) {
        m_status = FileStatus::forCondition(FileCondition::kInvalidName, "missing relative path");
    } else {
        m_absolutePath = baseDir / relativePath;
        m_status = posix_create_directories(m_absolutePath, perms);
    }
}

bool
tempo_utils::DirectoryMaker::isValid() const
{
    return m_status.isOk();
}

tempo_utils::Status
tempo_utils::DirectoryMaker::getStatus() const
{
    return m_status;
}

std::filesystem::path
tempo_utils::DirectoryMaker::getAbsolutePath() const
{
    return m_absolutePath;
}
