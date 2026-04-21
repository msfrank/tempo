#include <sys/fcntl.h>
#include <unistd.h>

#include <boost/endian/buffers.hpp>

#include <tempo_utils/file_appender.h>
#include <tempo_utils/file_result.h>
#include <tempo_utils/log_message.h>
#include <tempo_utils/posix_result.h>

#define BYTES_BUFFERED_BEFORE_FLUSH     512
#define POSIX_NUM_CLOSE_TRIES           3

static tempo_utils::Status
posix_open_file(
    std::string_view path,
    tempo_utils::FileAppenderMode mode,
    std::filesystem::perms perms,
    int *fd)
{
    TU_NOTNULL (fd);

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
posix_write(int fd, const tu_uint8 *data, tu_uint32 len)
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
    TU_RETURN_IF_NOT_OK (m_status);
    m_status = posix_write(m_fd, &u8, 1);
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendS8(tu_int8 s8)
{
    TU_RETURN_IF_NOT_OK (m_status);
    m_status = posix_write(m_fd, (const tu_uint8 *) &s8, 1);
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendU16(tu_uint16 u16)
{
    TU_RETURN_IF_NOT_OK (m_status);
    boost::endian::big_uint16_buf_t buf(u16);
    m_status = posix_write(m_fd, buf.data(), sizeof(buf));
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendU16LE(tu_uint16 u16)
{
    TU_RETURN_IF_NOT_OK (m_status);
    boost::endian::little_uint16_buf_t buf(u16);
    m_status = posix_write(m_fd, buf.data(), sizeof(buf));
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendS16(tu_int16 s16)
{
    TU_RETURN_IF_NOT_OK (m_status);
    boost::endian::big_int16_buf_t buf(s16);
    m_status = posix_write(m_fd, buf.data(), sizeof(buf));
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendS16LE(tu_int16 s16)
{
    TU_RETURN_IF_NOT_OK (m_status);
    boost::endian::little_int16_buf_t buf(s16);
    m_status = posix_write(m_fd, buf.data(), sizeof(buf));
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendU32(tu_uint32 u32)
{
    TU_RETURN_IF_NOT_OK (m_status);
    boost::endian::big_uint32_buf_t buf(u32);
    m_status = posix_write(m_fd, buf.data(), sizeof(buf));
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendU32LE(tu_uint32 u32)
{
    TU_RETURN_IF_NOT_OK (m_status);
    boost::endian::little_uint32_buf_t buf(u32);
    m_status = posix_write(m_fd, buf.data(), sizeof(buf));
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendS32(tu_int32 s32)
{
    TU_RETURN_IF_NOT_OK (m_status);
    boost::endian::big_int32_buf_t buf(s32);
    m_status = posix_write(m_fd, buf.data(), sizeof(buf));
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendS32LE(tu_int32 s32)
{
    TU_RETURN_IF_NOT_OK (m_status);
    boost::endian::little_int32_buf_t buf(s32);
    m_status = posix_write(m_fd, buf.data(), sizeof(buf));
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendU64(tu_uint64 u64)
{
    TU_RETURN_IF_NOT_OK (m_status);
    boost::endian::big_uint64_buf_t buf(u64);
    m_status = posix_write(m_fd, buf.data(), sizeof(buf));
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendU64LE(tu_uint64 u64)
{
    TU_RETURN_IF_NOT_OK (m_status);
    boost::endian::little_uint64_buf_t buf(u64);
    m_status = posix_write(m_fd, buf.data(), sizeof(buf));
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendS64(tu_int64 s64)
{
    TU_RETURN_IF_NOT_OK (m_status);
    boost::endian::big_int64_buf_t buf(s64);
    m_status = posix_write(m_fd, buf.data(), sizeof(buf));
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendS64LE(tu_int64 s64)
{
    TU_RETURN_IF_NOT_OK (m_status);
    boost::endian::little_int64_buf_t buf(s64);
    m_status = posix_write(m_fd, buf.data(), sizeof(buf));
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendF32(float f32)
{
    TU_RETURN_IF_NOT_OK (m_status);
    boost::endian::big_float32_buf_t buf(f32);
    m_status = posix_write(m_fd, buf.data(), sizeof(buf));
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendF32LE(float f32)
{
    TU_RETURN_IF_NOT_OK (m_status);
    boost::endian::little_float32_buf_t buf(f32);
    m_status = posix_write(m_fd, buf.data(), sizeof(buf));
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendF64(double f64)
{
    TU_RETURN_IF_NOT_OK (m_status);
    boost::endian::big_float64_buf_t buf(f64);
    m_status = posix_write(m_fd, buf.data(), sizeof(buf));
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendF64LE(double f64)
{
    TU_RETURN_IF_NOT_OK (m_status);
    boost::endian::little_float64_buf_t buf(f64);
    m_status = posix_write(m_fd, buf.data(), sizeof(buf));
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendBytes(const std::shared_ptr<const ImmutableBytes> &bytes)
{
    TU_RETURN_IF_NOT_OK (m_status);
    if (bytes == nullptr)
        return {};
    return appendBytes(bytes->getSpan());
}

tempo_utils::Status
tempo_utils::FileAppender::appendBytes(std::span<const tu_uint8> bytes)
{
    TU_RETURN_IF_NOT_OK (m_status);
    m_status = posix_write(m_fd, bytes.data(), bytes.size());
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::appendBytes(std::string_view str)
{
    TU_RETURN_IF_NOT_OK (m_status);
    m_status = posix_write(m_fd, (const tu_uint8 *) str.data(), str.size());
    return m_status;
}

tempo_utils::Status
tempo_utils::FileAppender::finish()
{
    TU_RETURN_IF_NOT_OK (m_status);
    if (m_fd < 0)
        return m_status;
    m_status = posix_close(m_fd);
    return m_status;
}
