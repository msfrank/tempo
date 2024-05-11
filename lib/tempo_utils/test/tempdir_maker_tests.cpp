#include <iostream>

#include <gtest/gtest.h>

#include <tempo_utils/tempo_utils.h>

TEST(TempdirMaker, TestCreateTempdir)
{
    tempo_utils::TempdirMaker tempdirMaker(std::filesystem::current_path(), "test.XXXXXXXX");

    ASSERT_TRUE (tempdirMaker.isValid());
    auto tempdir = tempdirMaker.getTempdir();
    std::cerr << "tempdir: " << tempdir;
    ASSERT_TRUE (std::filesystem::is_directory(tempdir));
    ASSERT_TRUE (tempdir.parent_path() == std::filesystem::current_path());
    ASSERT_EQ (tempdir.stem(), "test");
    ASSERT_TRUE (tempdir.has_extension());
    ASSERT_TRUE (tempdir.extension().string().size() == 9);

    ASSERT_TRUE (std::filesystem::remove(tempdir));
}

TEST(TempdirMaker, TestMissingParentDir)
{
    tempo_utils::TempdirMaker tempdirMaker("/invalid/base", "test.XXXXXXXX");
    ASSERT_FALSE (tempdirMaker.isValid());
    tempo_utils::FileStatus status;
    ASSERT_TRUE (tempdirMaker.getStatus().convertTo(status));
    ASSERT_EQ (tempo_utils::FileCondition::kParentDirectoryNotFound, status.getCondition());
}

TEST(TempdirMaker, TestInvalidTemplate)
{
    tempo_utils::TempdirMaker tempdirMaker("/invalid/base", "test.XXX");
    tempo_utils::FileStatus status;
    ASSERT_TRUE (tempdirMaker.getStatus().convertTo(status));
    ASSERT_EQ (tempo_utils::FileCondition::kInvalidTemplate, status.getCondition());
}