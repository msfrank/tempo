
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <tempo_utils/file_reader.h>
#include <tempo_utils/file_result.h>
#include <tempo_utils/memory_bytes.h>
#include <tempo_utils/posix_result.h>

#define READ_BUF_SIZE   512
#define NUM_RETRIES     3

static tempo_utils::Status
posix_read_file_completely(std::string_view path, std::vector<tu_uint8> &bytes)
{
    // attempt to open the file
    int fd = open(path.data(), O_RDONLY);
    if (fd < 0) {
        switch (errno) {
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

    // attempt to stat the file
    struct stat st;
    if (stat(path.data(), &st) < 0)
        return tempo_utils::PosixStatus::last("stat() failed");

    // resize the bytes vector to the length of the file
    bytes.resize(st.st_size);

    tu_uint8 *buf = bytes.data();
    size_t nread = 0;
    auto nleft = st.st_size - nread;
    int retries = NUM_RETRIES;

    // loop calling read until either nleft equals 0 or we encounter an error
    do {
        auto ret = read(fd, buf + nread, nleft < READ_BUF_SIZE? nleft : READ_BUF_SIZE);
        if (nread < 0) {
            switch (errno) {
                case EAGAIN:
                    if (retries == 0)
                        return tempo_utils::PosixStatus::last("read() failed");
                    retries--;
                    break;
                default:
                    return tempo_utils::PosixStatus::last("read() failed");
            }
        } else {
            nread += ret;
            nleft = st.st_size - nread;
            retries = NUM_RETRIES;
        }
    } while (nleft > 0);

    //
    close(fd);

    return tempo_utils::Status();
}

tempo_utils::FileReader::FileReader(const std::filesystem::path &path)
{
    m_absolutePath = absolute(path);

    std::vector<tu_uint8> bytes;
    m_status = posix_read_file_completely(path.c_str(), bytes);
    m_bytes = MemoryBytes::create(std::move(bytes));
}

bool
tempo_utils::FileReader::isValid() const
{
    return m_status.isOk();
}

tempo_utils::Status
tempo_utils::FileReader::getStatus() const
{
    return m_status;
}

std::filesystem::path
tempo_utils::FileReader::getAbsolutePath() const
{
    return m_absolutePath;
}

/**
 * Returns a shared ImmutableBytes instance containing the file contents if file reading was successful,
 * otherwise returns a shared_ptr containing nullptr if file reading failed.
 *
 * @return
 */
std::shared_ptr<const tempo_utils::ImmutableBytes>
tempo_utils::FileReader::getBytes() const
{
    return m_bytes;
}
