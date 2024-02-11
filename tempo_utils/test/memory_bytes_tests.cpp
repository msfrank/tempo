
#include <gtest/gtest.h>

#include <tempo_utils/memory_bytes.h>

TEST(MemoryBytes, TestCreate)
{
    std::vector<tu_uint8> src = {1, 2, 3};

    auto bytes = tempo_utils::MemoryBytes::create(std::move(src));
    ASSERT_TRUE (bytes != nullptr);
    ASSERT_EQ (3, bytes->getSize());
    auto *data = bytes->getData();
    ASSERT_EQ (1, data[0]);
    ASSERT_EQ (2, data[1]);
    ASSERT_EQ (3, data[2]);
}

TEST(MemoryBytes, TestCopyDataAndSize)
{
    std::vector<tu_uint8> src = {1, 2, 3};

    auto bytes = tempo_utils::MemoryBytes::copy(src);
    ASSERT_TRUE (bytes != nullptr);
    ASSERT_EQ (3, bytes->getSize());
    auto *data = bytes->getData();
    ASSERT_EQ (1, data[0]);
    ASSERT_EQ (2, data[1]);
    ASSERT_EQ (3, data[2]);
}

TEST(MemoryBytes, TestCopyInitializer)
{
    auto bytes = tempo_utils::MemoryBytes::copy({1, 2, 3});
    ASSERT_TRUE (bytes != nullptr);
    ASSERT_EQ (3, bytes->getSize());
    auto *data = bytes->getData();
    ASSERT_EQ (1, data[0]);
    ASSERT_EQ (2, data[1]);
    ASSERT_EQ (3, data[2]);
}

TEST(MemoryBytes, TestCopyStdStringView)
{
    std::string src = {1, 2, 3};

    auto bytes = tempo_utils::MemoryBytes::copy(src);
    ASSERT_TRUE (bytes != nullptr);
    ASSERT_EQ (3, bytes->getSize());
    auto *data = bytes->getData();
    ASSERT_EQ (1, data[0]);
    ASSERT_EQ (2, data[1]);
    ASSERT_EQ (3, data[2]);
}
