
#include <ostream>

#include <absl/strings/str_cat.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <tempo_utils/rope.h>

#include "rope_test_parameters.h"

class Rope : public ::testing::Test {};

TEST_F(Rope, MakeRopeWithSingleElement)
{
    tempo_utils::Rope<int> rope{42};

    ASSERT_EQ (1, rope.numElements());
    ASSERT_EQ (42, rope.getElement(0));
    ASSERT_EQ (42, rope.elementAt(0));
    ASSERT_EQ (0, rope.getDepth());
}

TEST_F(Rope, MakeRopeWithMultipleElements)
{
    tempo_utils::Rope<int> rope{1, 2, 3, 4, 5};

    ASSERT_EQ (5, rope.numElements());
    ASSERT_EQ (4, rope.getElement(3));
    ASSERT_EQ (4, rope.elementAt(3));
    ASSERT_EQ (0, rope.getDepth());
}

TEST_F(Rope, AppendRope)
{
    tempo_utils::Rope<int> rope1{1, 2, 3};
    tempo_utils::Rope<int> rope2{4, 5};

    auto rope = rope1.append(rope2);

    ASSERT_EQ (5, rope.numElements());
    ASSERT_EQ (4, rope.getElement(3));
    ASSERT_EQ (4, rope.elementAt(3));
    ASSERT_EQ (1, rope.getDepth());
}

TEST_F(Rope, PrependRope)
{
    tempo_utils::Rope<int> rope1{1, 2, 3};
    tempo_utils::Rope<int> rope2{4, 5};

    auto rope = rope1.prepend(rope2);

    ASSERT_EQ (5, rope.numElements());
    ASSERT_EQ (2, rope.getElement(3));
    ASSERT_EQ (2, rope.elementAt(3));
    ASSERT_EQ (1, rope.getDepth());
}

TEST_F(Rope, SplitRopeWithSingleLeafNode)
{
    tempo_utils::Rope<int> rope{1, 2, 3, 4, 5};
    auto pair = rope.split(2);

    auto &left = pair.first;
    ASSERT_EQ (2, left.numElements());
    ASSERT_EQ (2, left.getElement(1));

    auto &right = pair.second;
    ASSERT_EQ (3, right.numElements());
    ASSERT_EQ (4, right.getElement(1));
}

TEST_F(Rope, SplitRopeWithConcatOfTwoLeafNodes)
{
    tempo_utils::Rope<int> rope1{1, 2, 3};
    tempo_utils::Rope<int> rope2{4, 5};
    auto rope = rope1.append(rope2);

    auto pair = rope.split(2);

    auto &left = pair.first;
    ASSERT_EQ (2, left.numElements());
    ASSERT_EQ (2, left.getElement(1));

    auto &right = pair.second;
    ASSERT_EQ (3, right.numElements());
    ASSERT_EQ (4, right.getElement(1));
}

TEST_F(Rope, SplitRopeWithConcatOfTwoConcatfNodes)
{
    tempo_utils::Rope<int> rope1{1, 2};
    tempo_utils::Rope<int> rope2{3, 4};
    tempo_utils::Rope<int> rope3{5, 6};
    tempo_utils::Rope<int> rope4{7, 8};
    auto concat1 = rope1.append(rope2);
    auto concat2 = rope3.append(rope4);
    auto rope = concat1.append(concat2);

    auto pair = rope.split(3);

    auto &left = pair.first;
    ASSERT_EQ (3, left.numElements());
    ASSERT_EQ (2, left.getElement(1));

    auto &right = pair.second;
    ASSERT_EQ (5, right.numElements());
    ASSERT_EQ (6, right.getElement(2));
}

TEST_F(Rope, SubspanOfRopeWithConcatOfTwoConcatfNodes)
{
    tempo_utils::Rope<int> rope1{1, 2};
    tempo_utils::Rope<int> rope2{3, 4};
    tempo_utils::Rope<int> rope3{5, 6};
    tempo_utils::Rope<int> rope4{7, 8};
    auto concat1 = rope1.append(rope2);
    auto concat2 = rope3.append(rope4);
    auto rope = concat1.append(concat2);

    auto sub = rope.subspan(3, 3);
    ASSERT_EQ (3, sub.numElements());
    ASSERT_EQ (4, sub.getElement(0));
    ASSERT_EQ (5, sub.getElement(1));
    ASSERT_EQ (6, sub.getElement(2));
}

class RopeLayouts : public ::testing::Test, public ::testing::WithParamInterface<RopeTestParameter> {};

INSTANTIATE_TEST_SUITE_P(
    Operations,
    RopeLayouts,
    ::testing::Values(
        leaf_node_containing_1_element(),
        leaf_node_containing_5_elements(),
        concat_of_leaf_nodes(),
        concat_of_two_concats_of_leaf_nodes()
        ),
    [](const testing::TestParamInfo<RopeLayouts::ParamType>& p) {
      return p.param.paramName.empty()? absl::StrCat("param#", p.index) : p.param.paramName;
    }
);

TEST_P(RopeLayouts, IterateRopeChunks)
{
    auto &param = GetParam();

    auto it = param.rope.iterateChunks();

    for (const auto &expected : param.totalChunks) {
        ASSERT_TRUE (it.hasNext());
        tempo_utils::RopeChunk<std::string> chunk;
        ASSERT_TRUE (it.getNext(chunk));
        std::vector actual(chunk.cbegin(), chunk.cend());
        ASSERT_THAT (actual, ::testing::ElementsAreArray(expected));
    }

    ASSERT_FALSE (it.hasNext());
}

TEST_P(RopeLayouts, IterateRopeElements)
{
    auto &param = GetParam();

    auto it = param.rope.iterateElements();

    for (const auto &expected : param.totalElements) {
        ASSERT_TRUE (it.hasNext());
        std::string actual;
        ASSERT_TRUE (it.getNext(actual));
        ASSERT_EQ (expected, actual);
    }

    ASSERT_FALSE (it.hasNext());
}

TEST_P(RopeLayouts, SplitRope)
{
    auto &param = GetParam();

    auto rope = param.rope;

    auto size = param.totalElements.size();
    for (int i = 0; i < size; ++i) {
        auto split = rope.split(i);

        auto leftRope = split.first;
        ASSERT_EQ (i, leftRope.numElements());
        for (int j = 0; j < i; ++j) {
            std::string element = leftRope.elementAt(j);
            ASSERT_THAT (element, ::testing::Eq(param.totalElements[j]))
                << "left rope element " << j << "does not match";
        }

        auto rightRope = split.second;
        ASSERT_EQ (size - i, rightRope.numElements());
        for (int j = 0; j < size - i; ++j) {
            std::string element = rightRope.elementAt(j);
            ASSERT_THAT (element, ::testing::Eq(param.totalElements[i + j]))
                << "right rope element " << j << "does not match";
        }
    }
}

TEST_P(RopeLayouts, SubspanOfRope)
{
    auto &param = GetParam();

    auto rope = param.rope;

    auto size = param.totalElements.size();
    for (int offset = 0; offset < size; ++offset)
    {
        for (int count = 0; count < size - offset; ++count) {
            auto sub = rope.subspan(offset, count);

            ASSERT_EQ (count, sub.numElements());
            for (int i = 0; i < count; ++i) {
                std::string element = sub.elementAt(i);
                ASSERT_THAT (element, ::testing::Eq(param.totalElements[offset + i]))
                    << "subspan rope element " << i << " does not match"
                    << " (size=" << size << " offset=" << offset << " count=" << count << ")";
            }
        }
    }
}