
#include <gtest/gtest.h>

#include <tempo_utils/memory_bytes.h>
#include <tempo_utils/slice.h>

TEST(Slice, TestConstructSliceWithPos)
{
    auto bytes = tempo_utils::MemoryBytes::copy({1, 2, 3});
    auto slice = tempo_utils::Slice(bytes, 1);
    ASSERT_FALSE (slice.isEmpty());
    ASSERT_EQ (2, slice.getSize());
    ASSERT_EQ (2, slice.getByte(0));
    ASSERT_EQ (3, slice.getByte(1));
}

TEST(Slice, TestConstructSliceWithPosOutOfRange)
{
    auto bytes = tempo_utils::MemoryBytes::copy({1, 2, 3});
    auto slice = tempo_utils::Slice(bytes, 4);
    ASSERT_TRUE (slice.isEmpty());
    ASSERT_EQ (0, slice.getSize());
}

TEST(Slice, TestConstructSliceWithPosAndCount)
{
    auto bytes = tempo_utils::MemoryBytes::copy({1, 2, 3});
    auto slice = tempo_utils::Slice(bytes, 1, 1);
    ASSERT_FALSE (slice.isEmpty());
    ASSERT_EQ (1, slice.getSize());
    ASSERT_EQ (2, slice.getByte(0));
}

TEST(Slice, TestConstructSliceWithPosOutOfRangeAndCount)
{
    auto bytes = tempo_utils::MemoryBytes::copy({1, 2, 3});
    auto slice = tempo_utils::Slice(bytes, 4, 1);
    ASSERT_TRUE (slice.isEmpty());
    ASSERT_EQ (0, slice.getSize());
}

TEST(Slice, TestConstructSliceWithPosAndCountOutOfRange)
{
    auto bytes = tempo_utils::MemoryBytes::copy({1, 2, 3});
    auto slice = tempo_utils::Slice(bytes, 1, 3);
    ASSERT_FALSE (slice.isEmpty());
    ASSERT_EQ (2, slice.getSize());
    ASSERT_EQ (2, slice.getByte(0));
    ASSERT_EQ (3, slice.getByte(1));
}

TEST(Slice, TestConstructSliceWithPosOutOfRangeAndCountOutOfRange)
{
    auto bytes = tempo_utils::MemoryBytes::copy({1, 2, 3});
    auto slice = tempo_utils::Slice(bytes, 4, 5);
    ASSERT_TRUE (slice.isEmpty());
    ASSERT_EQ (0, slice.getSize());
}

TEST(Slice, TestCreateSlice)
{
    auto bytes = tempo_utils::MemoryBytes::copy({1, 2, 3});
    auto slice = tempo_utils::Slice(bytes);
    ASSERT_FALSE (slice.isEmpty());
    ASSERT_EQ (3, slice.getSize());
    ASSERT_EQ (1, slice.getByte(0));
    ASSERT_EQ (2, slice.getByte(1));
    ASSERT_EQ (3, slice.getByte(2));
}

TEST(Slice, TestCreateSubSlice)
{
    auto bytes = tempo_utils::MemoryBytes::copy({1, 2, 3});
    auto slice = tempo_utils::Slice(bytes);
    auto subslice = slice.slice(1, 2);
    ASSERT_FALSE (slice.isEmpty());
    ASSERT_EQ (2, subslice.getSize());
    ASSERT_EQ (2, subslice.getByte(0));
    ASSERT_EQ (3, subslice.getByte(1));
}

TEST(Slice, TestToImmutableBytes)
{
    auto bytes = tempo_utils::MemoryBytes::copy({1, 2, 3});
    auto slice = tempo_utils::Slice(bytes, 1, 2);
    auto toBytes = slice.toImmutableBytes();
    ASSERT_TRUE (toBytes != nullptr);
    ASSERT_EQ (2, toBytes->getSize());
    auto *data = toBytes->getData();
    ASSERT_EQ (2, data[0]);
    ASSERT_EQ (3, data[1]);
}
