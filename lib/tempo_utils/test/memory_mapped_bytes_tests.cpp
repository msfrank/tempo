
#include <gtest/gtest.h>

#include <tempo_utils/file_result.h>
#include <tempo_utils/file_utilities.h>
#include <tempo_utils/file_writer.h>
#include <tempo_utils/memory_mapped_bytes.h>
#include <tempo_utils/tempfile_maker.h>

TEST(MemoryMappedBytes, TestOpenSucceeds)
{
    std::vector<tu_uint8> src = {1, 2, 3};
    tempo_utils::TempfileMaker tempfileMaker(std::filesystem::current_path(), "test.XXXXXXXX", src);
    ASSERT_TRUE (tempfileMaker.isValid());

    auto openResult = tempo_utils::MemoryMappedBytes::open(tempfileMaker.getTempfile());
    auto bytes = openResult.orElseThrow();
    ASSERT_TRUE (bytes != nullptr);
    ASSERT_EQ (3, bytes->getSize());
    auto *data = bytes->getData();
    ASSERT_EQ (1, data[0]);
    ASSERT_EQ (2, data[1]);
    ASSERT_EQ (3, data[2]);
}

TEST(MemoryMappedBytes, TestOpenFailsWhenFileNotFound)
{
    auto noSuchFile = std::filesystem::current_path() / tempo_utils::generate_name("notfound.XXXXXXXX");
    auto openResult = tempo_utils::MemoryMappedBytes::open(noSuchFile);
    ASSERT_TRUE (openResult.isStatus());
    ASSERT_TRUE (openResult.getStatus().matchesCondition(tempo_utils::FileCondition::kFileNotFound));
}

TEST(MemoryMappedBytes, TestOpenFailsWhenAccessDenied)
{
    auto path = std::filesystem::current_path() / tempo_utils::generate_name("noperms.XXXXXXXX");
    tempo_utils::FileWriter fileWriter(path, "", tempo_utils::FileWriterMode::CREATE_ONLY, std::filesystem::perms(0200));
    ASSERT_TRUE (fileWriter.isValid());

    auto openResult = tempo_utils::MemoryMappedBytes::open(fileWriter.getAbsolutePath());
    ASSERT_TRUE (openResult.isStatus());
    ASSERT_TRUE (openResult.getStatus().matchesCondition(tempo_utils::FileCondition::kAccessDenied));
}
