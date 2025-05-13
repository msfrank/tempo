
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <tempo_utils/file_result.h>
#include <tempo_utils/file_writer.h>
#include <tempo_utils/posix_result.h>

#define POSIX_NUM_CLOSE_TRIES   3

static tempo_utils::Status
posix_write_file_completely(
    std::string_view path,
    const char *data,
    tu_uint32 size,
    tempo_utils::FileWriterMode mode,
    std::filesystem::perms perms)
{
    int fd = -1;

    switch (mode) {
        case tempo_utils::FileWriterMode::APPEND:
            //fp = std::fopen(path.data(), "a");
            fd = open(path.data(), O_WRONLY | O_CREAT | O_APPEND, (mode_t) perms);
            break;
        case tempo_utils::FileWriterMode::CREATE_OR_OVERWRITE:
            //fp = std::fopen(path.data(), "w");
            fd = open(path.data(), O_WRONLY | O_CREAT | O_TRUNC, (mode_t) perms);
            break;
        case tempo_utils::FileWriterMode::CREATE_ONLY:
            //fp = std::fopen(path.data(), "wx");
            fd = open(path.data(), O_WRONLY | O_CREAT | O_EXCL, (mode_t) perms);
            break;
        case tempo_utils::FileWriterMode::OVERWRITE_ONLY:
            //fp = std::fopen(path.data(), "r+");
            fd = open(path.data(), O_WRONLY | O_TRUNC, (mode_t) perms);
            break;
    }

    if (fd < 0) {
        switch (errno) {
            case EEXIST:
                return tempo_utils::FileStatus::forCondition(
                    tempo_utils::FileCondition::kFileExists,
                    "file {} already exists", path);
            case ENOENT:
                return tempo_utils::FileStatus::forCondition(
                    tempo_utils::FileCondition::kFileNotFound,
                    "file {} not found", path);
            case EACCES:
                return tempo_utils::FileStatus::forCondition(
                    tempo_utils::FileCondition::kAccessDenied,
                    "access denied for {}", path);
            default:
                return tempo_utils::PosixStatus::fromError(errno);
        }
    }

    auto *curr = data;
    size_t nleft = size;
    tempo_utils::Status status;
    while (nleft > 0 && status.isOk()) {
        ssize_t nwritten = write(fd, curr, nleft);
        if (nwritten < 0) {
            status = tempo_utils::PosixStatus::fromError(errno);
        }
        curr += nwritten;
        nleft -= nwritten;
    }

    for (int attempt = 0; attempt < POSIX_NUM_CLOSE_TRIES; attempt++) {
        if (close(fd) == 0)
            break;
        if (errno == EINTR)
            continue;
        return tempo_utils::PosixStatus::fromError(errno);
    }

    return status;
}

tempo_utils::FileWriter::FileWriter(
    const std::filesystem::path &path,
    std::shared_ptr<ImmutableBytes> bytes,
    FileWriterMode mode,
    std::filesystem::perms perms)
    : m_mode(mode)
{
    m_absolutePath = absolute(path);
    const char *data = nullptr;
    tu_uint32 size = 0;
    if (bytes != nullptr) {
        data = (const char *) bytes->getData();
        size = bytes->getSize();
    }
    m_status = posix_write_file_completely(path.c_str(), data, size, mode, perms);
}

tempo_utils::FileWriter::FileWriter(
    const std::filesystem::path &path,
    std::span<const tu_uint8> bytes,
    FileWriterMode mode,
    std::filesystem::perms perms)
    : m_mode(mode)
{
    m_absolutePath = absolute(path);
    m_status = posix_write_file_completely(path.c_str(), (const char *) bytes.data(), bytes.size(), mode, perms);
}

tempo_utils::FileWriter::FileWriter(
    const std::filesystem::path &path,
    std::string_view str,
    FileWriterMode mode,
    std::filesystem::perms perms)
    : m_mode(mode)
{
    m_absolutePath = absolute(path);
    m_status = posix_write_file_completely(path.c_str(), str.data(), str.size(), mode, perms);
}

bool
tempo_utils::FileWriter::isValid() const
{
    return m_status.isOk();
}

tempo_utils::Status
tempo_utils::FileWriter::getStatus() const
{
    return m_status;
}

std::filesystem::path
tempo_utils::FileWriter::getAbsolutePath() const
{
    return m_absolutePath;
}

tempo_utils::FileWriterMode
tempo_utils::FileWriter::getMode()
{
    return m_mode;
}