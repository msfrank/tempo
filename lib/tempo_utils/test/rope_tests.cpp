
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

class RopeLayouts : public ::testing::Test, public ::testing::WithParamInterface<RopeTestParameter> {};

INSTANTIATE_TEST_SUITE_P(
    Operations,
    RopeLayouts,
    ::testing::Values(
        empty_rope(),
        leaf_node_containing_1_element(),
        leaf_node_containing_5_elements(),
        concat_of_leaf_nodes(),
        concat_of_two_concats_of_leaf_nodes()
        ),
    [](const testing::TestParamInfo<RopeLayouts::ParamType>& p) {
      return p.param.paramName.empty()? absl::StrCat("param#", p.index) : p.param.paramName;
    }
);

TEST_P(RopeLayouts, PrependRope)
{
    auto &param = GetParam();

    std::vector<std::string> prependElements{"a", "b", "c"};

    auto rope = param.rope;
    tempo_utils::Rope<std::string> other(prependElements.cbegin(), prependElements.cend());

    auto prepend = rope.prepend(other);
    ASSERT_EQ (param.totalElements.size() + other.numElements(), prepend.numElements());

    auto expectedElements = param.totalElements;
    expectedElements.insert(expectedElements.cbegin(), prependElements.cbegin(), prependElements.cend());
    auto actualElements = prepend.toVector();
    ASSERT_THAT (actualElements, ::testing::ContainerEq(expectedElements));
}

TEST_P(RopeLayouts, AppendRope)
{
    auto &param = GetParam();

    std::vector<std::string> appendElements{"a", "b", "c"};

    auto rope = param.rope;
    tempo_utils::Rope<std::string> other(appendElements.cbegin(), appendElements.cend());

    auto append = rope.append(other);
    ASSERT_EQ (param.totalElements.size() + other.numElements(), append.numElements());

    auto expectedElements = param.totalElements;
    expectedElements.insert(expectedElements.cend(), appendElements.cbegin(), appendElements.cend());
    auto actualElements = append.toVector();
    ASSERT_THAT (actualElements, ::testing::ContainerEq(expectedElements));
}

TEST_P(RopeLayouts, InsertRope)
{
    auto &param = GetParam();

    std::vector<std::string> insertElements{"a", "b", "c"};

    auto rope = param.rope;
    tempo_utils::Rope<std::string> other(insertElements.cbegin(), insertElements.cend());

    auto size = param.totalElements.size();

    for (int offset = 0; offset < size; ++offset) {
        auto insert = rope.insert(offset, other);

        ASSERT_EQ (size + other.numElements(), insert.numElements());

        auto expectedElements = param.totalElements;
        auto it = expectedElements.cbegin();
        std::advance(it, offset);
        expectedElements.insert(it, insertElements.cbegin(), insertElements.cend());

        auto actualElements = insert.toVector();
        ASSERT_THAT (actualElements, ::testing::ContainerEq(expectedElements));
    }
}

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

        auto leftBegin = param.totalElements.cbegin();
        auto leftEnd = param.totalElements.cbegin();
        std::advance(leftEnd, i);
        std::vector leftExpected(leftBegin, leftEnd);
        auto leftActual = leftRope.toVector();
        ASSERT_THAT (leftActual, ::testing::ContainerEq(leftExpected));

        auto rightRope = split.second;
        ASSERT_EQ (size - i, rightRope.numElements());

        auto rightBegin = param.totalElements.cbegin();
        std::advance(rightBegin, i);
        auto rightEnd = param.totalElements.cend();
        std::vector rightExpected(rightBegin, rightEnd);
        auto rightActual = rightRope.toVector();
        ASSERT_THAT (rightActual, ::testing::ContainerEq(rightExpected));
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

            auto first = param.totalElements.cbegin();
            std::advance(first, offset);
            auto last = param.totalElements.cbegin();
            std::advance(last, offset + count);
            std::vector expectedElements(first, last);

            auto actualElements = sub.toVector();
            ASSERT_THAT (actualElements, ::testing::ContainerEq(expectedElements));
        }
    }
}

TEST_P(RopeLayouts, RemoveFromRope)
{
    auto &param = GetParam();

    auto rope = param.rope;

    auto size = param.totalElements.size();
    for (int offset = 0; offset < size; ++offset)
    {
        for (int count = 0; count < size - offset; ++count) {
            auto rem = rope.remove(offset, count);

            ASSERT_EQ (size - count, rem.numElements());

            auto expectedElements = param.totalElements;
            auto first = expectedElements.cbegin();
            std::advance(first, offset);
            auto last = expectedElements.cbegin();
            std::advance(last, offset + count);
            expectedElements.erase(first, last);

            auto actualElements = rem.toVector();
            ASSERT_THAT (actualElements, ::testing::ContainerEq(expectedElements));
        }
    }
}
