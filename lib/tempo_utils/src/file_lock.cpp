
#include <absl/time/clock.h>
#include <tempo_utils/file_lock.h>
#include <tempo_utils/file_result.h>

constexpr double kSpinDelay = 10.0;

tempo_utils::FileLock::FileLock(
    const std::filesystem::path &path,
    int lockTimeoutInMs,
    std::filesystem::perms perms)
    : FileLock(path, "", lockTimeoutInMs, perms)
{
}

bool
try_create_lock(
    const std::filesystem::path &path,
    std::string_view str,
    std::filesystem::perms perms,
    std::unique_ptr<tempo_utils::FileWriter> &writer,
    tempo_utils::Status &status)
{
    auto writer_ = std::make_unique<tempo_utils::FileWriter>(path, str, tempo_utils::FileWriterMode::CREATE_ONLY, perms);
    if (writer_->isValid()) {
        writer = std::move(writer_);
        return true;
    }

    auto status_ = writer->getStatus();
    if (!status_.matchesCondition(tempo_utils::FileCondition::kFileExists)) {
        status = std::move(status_);
        return true;
    }

    return false;
}

tempo_utils::FileLock::FileLock(
    const std::filesystem::path &path,
    std::string_view str,
    int lockTimeoutInMs,
    std::filesystem::perms perms)
{
    int spinsLeft;
    double spinDelay;

    // calculate the number of spins and the spin delay
    if (lockTimeoutInMs < 0) {
        spinsLeft = -1;
        spinDelay = kSpinDelay;
    } else if (lockTimeoutInMs == 0) {
        spinsLeft = 0;
        spinDelay = 0;
    } else if (lockTimeoutInMs <= kSpinDelay) {
        spinsLeft = 1;
        spinDelay = kSpinDelay;
    } else {
        auto intervals = static_cast<double>(lockTimeoutInMs) / kSpinDelay;
        spinsLeft = std::floor(intervals);
        spinDelay = static_cast<double>(lockTimeoutInMs) / static_cast<double>(spinsLeft);
    }

    do {
        if (try_create_lock(path, str, perms, m_writer, m_status))
            return;
        if (spinsLeft > 0) {
            spinsLeft--;
        }
        absl::SleepFor(absl::Milliseconds(spinDelay));
    } while (spinsLeft != 0);
}

tempo_utils::FileLock::~FileLock()
{
    unlock();
}

bool
tempo_utils::FileLock::isValid() const
{
    return m_status.isOk();
}

tempo_utils::Status
tempo_utils::FileLock::getStatus() const
{
    return m_status;
}

bool
tempo_utils::FileLock::isLocked() const
{
    return m_writer != nullptr;
}

tempo_utils::Status
tempo_utils::FileLock::unlock()
{
    if (m_writer == nullptr)
        return {};
    auto path = m_writer->getAbsolutePath();
    m_writer.reset();

    std::error_code ec;
    if (!std::filesystem::remove(path, ec))
        return FileStatus::forCondition(FileCondition::kSystemInvariant,
            "failed to remove lockfile {}; {}", path.string(), ec.message());
    return {};
}