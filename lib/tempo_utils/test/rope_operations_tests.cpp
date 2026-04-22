
#include <gtest/gtest.h>

#include <tempo_utils/rope_operations.h>

class RopeOperations : public ::testing::Test {};

TEST_F(RopeOperations, MakeRopeWithSingleElement)
{
    auto rope = tempo_utils::make_rope<int>(42);
    ASSERT_EQ (1, rope->getWeight());
    ASSERT_EQ (1, rope->getSize());
    ASSERT_EQ (0, rope->getDepth());
    ASSERT_TRUE (rope->isBalanced());
}

TEST_F(RopeOperations, MakeRopeWithMultipleElements)
{
    std::vector<int> elements{1, 2, 3, 4, 5};
    auto rope = tempo_utils::make_rope<int>(std::move(elements));
    ASSERT_EQ (5, rope->getWeight());
    ASSERT_EQ (5, rope->getSize());
    ASSERT_EQ (0, rope->getDepth());
    ASSERT_TRUE (rope->isBalanced());
}

TEST_F(RopeOperations, ConcatTwoLeafNodes)
{
    std::vector<int> elements1{1, 2, 3};
    auto leaf1 = tempo_utils::make_rope<int>(std::move(elements1));

    std::vector<int> elements2{4, 5};
    auto leaf2 = tempo_utils::make_rope<int>(std::move(elements2));

    auto rope = tempo_utils::concat_rope(leaf1, leaf2);
    ASSERT_EQ (3, rope->getWeight());
    ASSERT_EQ (5, rope->getSize());
    ASSERT_EQ (1, rope->getDepth());
    ASSERT_TRUE (rope->isBalanced());
}

TEST_F(RopeOperations, SplitLeafNode)
{
    std::vector<int> elements{1, 2, 3, 4, 5};
    auto rope = tempo_utils::make_rope<int>(std::move(elements));
    auto pair = tempo_utils::split_rope(rope, 2);
    auto &lhs = pair.first;
    auto &rhs = pair.second;
    ASSERT_EQ (2, lhs->getSize());
    ASSERT_EQ (3, rhs->getSize());
}
