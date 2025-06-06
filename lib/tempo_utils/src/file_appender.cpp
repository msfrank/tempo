#include <sys/fcntl.h>
#include <unistd.h>

#include <tempo_utils/big_endian.h>
#include <tempo_utils/file_appender.h>
#include <tempo_utils/file_result.h>
#include <tempo_utils/posix_result.h>

#define POSIX_NUM_CLOSE_TRIES   3

static tempo_utils::Status
posix_open_file(
    std::string_view path,
    tempo_utils::FileAppenderMode mode,
    std::filesystem::perms perms,
    int *fd)
{
    TU_ASSERT (fd != nullptr);

    int _fd = -1;
    switch (mode) {
        case tempo_utils::FileAppenderMode::APPEND:
            _fd = open(path.data(), O_WRONLY | O_APPEND, (mode_t) perms);
            break;
        case tempo_utils::FileAppenderMode::CREATE_OR_APPEND:
            _fd = open(path.data(), O_WRONLY | O_CREAT | O_APPEND, (mode_t) perms);
            break;
        case tempo_utils::FileAppenderMode::CREATE_OR_OVERWRITE:
            _fd = open(path.data(), O_WRONLY | O_CREAT | O_TRUNC, (mode_t) perms);
            break;
        case tempo_utils::FileAppenderMode::CREATE_ONLY:
            _fd = open(path.data(), O_WRONLY | O_CREAT | O_EXCL, (mode_t) perms);
            break;
    }

    if (_fd < 0) {
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

    *fd = _fd;
    return {};
}

static tempo_utils::Status
posix_write(int fd, const char *data, tu_uint32 len)
{
    TU_ASSERT (fd >= 0);

    auto *curr = data;
    size_t nleft = len;
    tempo_utils::Status status;
    while (nleft > 0 && status.isOk()) {
        ssize_t nwritten = write(fd, curr, nleft);
        if (nwritten < 0) {
            status = tempo_utils::PosixStatus::fromError(errno);
        }
        curr += nwritten;
        nleft -= nwritten;
    }
    return status;
}

static tempo_utils::Status
posix_close(int fd)
{
    if (fd < 0)
        return {};

    for (int attempt = 0; attempt < POSIX_NUM_CLOSE_TRIES; attempt++) {
        if (close(fd) == 0)
            break;
        if (errno == EINTR)
            continue;
        return tempo_utils::PosixStatus::fromError(errno);
    }
    return {};
}

tempo_utils::FileAppender::FileAppender(
    const std::filesystem::path &path,
    FileAppenderMode mode,
    std::filesystem::perms perms)
    : m_mode(mode),
      m_fd(-1)
{
    m_absolutePath = absolute(path);
    m_status = posix_open_file(path.c_str(), m_mode, perms, &m_fd);
}

tempo_utils::FileAppender::FileAppender(const std::filesystem::path &path)
    : FileAppender(path, FileAppenderMode::CREATE_OR_APPEND)
{
}

tempo_utils::FileAppender::~FileAppender()
{
    posix_close(m_fd);
}

bool
tempo_utils::FileAppender::isValid() const
{
    return m_status.isOk();
}

tempo_utils::Status
tempo_utils::FileAppender::getStatus() const
{
    return m_status;
}

std::filesystem::path
tempo_utils::FileAppender::getAbsolutePath() const
{
    return m_absolutePath;
}

tempo_utils::FileAppenderMode
tempo_utils::FileAppender::getMode()
{
    return m_mode;
}

tempo_utils::Status
tempo_utils::FileAppender::appendU8(tu_uint8 u8)
{
    if (m_status.isOk()) {
        m_status = posix_write(m_fd, (const char *) &u8, 1);
    }
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendU16(tu_uint16 u16)
{
    if (m_status.isOk()) {
        u16 = H_TO_BE16(u16);
        m_status = posix_write(m_fd, (const char *) &u16, sizeof(u16));
    }
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendU32(tu_uint32 u32)
{
    if (m_status.isOk()) {
        auto _u32 = H_TO_BE32(u32);
        m_status = posix_write(m_fd, (const char *) &_u32, sizeof(_u32));
    }
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendBytes(std::shared_ptr<const ImmutableBytes> bytes)
{
    if (bytes == nullptr)
        return {};
    return appendBytes(bytes->getSpan());
}

tempo_utils::Status
tempo_utils::FileAppender::appendBytes(std::span<const tu_uint8> bytes)
{
    if (m_status.isOk()) {
        m_status = posix_write(m_fd, (const char *) bytes.data(), bytes.size());
    }
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendBytes(std::string_view str)
{
    if (m_status.isOk()) {
        m_status = posix_write(m_fd, str.data(), str.size());
    }
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::finish()
{
    if (m_fd < 0)
        return m_status;
    m_status = posix_close(m_fd);
    return m_status;
}
