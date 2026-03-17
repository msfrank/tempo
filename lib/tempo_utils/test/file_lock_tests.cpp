#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_utils/file_lock.h>
#include <tempo_utils/file_utilities.h>

class FileLock : public ::testing::Test {
protected:
    std::filesystem::path lockFilePath;
    void SetUp() override {
        lockFilePath = std::filesystem::current_path() / tempo_utils::generate_name("lockFile.XXXXXXXX");
    }
    void TearDown() override {
        if (std::filesystem::exists(lockFilePath)) {
            std::filesystem::remove(lockFilePath);
        }
    }
};

TEST_F(FileLock, CreateLockFileAndUnlock)
{
    auto fileLock = std::make_unique<tempo_utils::FileLock>(lockFilePath);
    ASSERT_TRUE (fileLock->isValid());
    ASSERT_TRUE (std::filesystem::is_regular_file(lockFilePath));
    ASSERT_TRUE (fileLock->isLocked());

    auto status = fileLock->unlock();
    ASSERT_TRUE (status.isOk());
    ASSERT_FALSE (std::filesystem::exists(lockFilePath));
    ASSERT_FALSE (fileLock->isLocked());
}
