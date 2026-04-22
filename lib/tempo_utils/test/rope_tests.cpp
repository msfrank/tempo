
#include <gtest/gtest.h>

#include <tempo_utils/rope.h>

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

TEST_F(Rope, IterateRopeChunksWithSingleLeafNode)
{
    tempo_utils::Rope<int> rope{1, 2, 3, 4, 5};

    auto it = rope.iterateChunks();

    ASSERT_TRUE (it.hasNext());
    tempo_utils::RopeChunk<int> chunk1;
    ASSERT_TRUE (it.getNext(chunk1));
    ASSERT_EQ (5, chunk1.numElements());
    ASSERT_FALSE (it.hasNext());
}

TEST_F(Rope, IterateRopeChunksWithConcatOfTwoLeafNodes)
{
    tempo_utils::Rope<int> rope1{1, 2, 3};
    tempo_utils::Rope<int> rope2{4, 5};
    auto rope = rope1.append(rope2);

    auto it = rope.iterateChunks();

    ASSERT_TRUE (it.hasNext());
    tempo_utils::RopeChunk<int> chunk1;
    ASSERT_TRUE (it.getNext(chunk1));
    ASSERT_EQ (3, chunk1.numElements());

    ASSERT_TRUE (it.hasNext());
    tempo_utils::RopeChunk<int> chunk2;
    ASSERT_TRUE (it.getNext(chunk2));
    ASSERT_EQ (2, chunk2.numElements());
    ASSERT_FALSE (it.hasNext());
}

TEST_F(Rope, IterateRopeChunksWithConcatOfTwoConcatfNodes)
{
    tempo_utils::Rope<int> rope1{1, 2};
    tempo_utils::Rope<int> rope2{3, 4};
    tempo_utils::Rope<int> rope3{5, 6};
    tempo_utils::Rope<int> rope4{7, 8};
    auto concat1 = rope1.append(rope2);
    auto concat2 = rope3.append(rope4);
    auto rope = concat1.append(concat2);

    auto it = rope.iterateChunks();

    ASSERT_TRUE (it.hasNext());
    tempo_utils::RopeChunk<int> chunk1;
    ASSERT_TRUE (it.getNext(chunk1));
    ASSERT_EQ (2, chunk1.numElements());

    ASSERT_TRUE (it.hasNext());
    tempo_utils::RopeChunk<int> chunk2;
    ASSERT_TRUE (it.getNext(chunk2));
    ASSERT_EQ (2, chunk2.numElements());

    ASSERT_TRUE (it.hasNext());
    tempo_utils::RopeChunk<int> chunk3;
    ASSERT_TRUE (it.getNext(chunk3));
    ASSERT_EQ (2, chunk3.numElements());

    ASSERT_TRUE (it.hasNext());
    tempo_utils::RopeChunk<int> chunk4;
    ASSERT_TRUE (it.getNext(chunk4));
    ASSERT_EQ (2, chunk4.numElements());
    ASSERT_FALSE (it.hasNext());
}

TEST_F(Rope, IterateRopeElementsWithSingleLeafNode)
{
    tempo_utils::Rope<int> rope{1, 2, 3, 4, 5};

    auto it = rope.iterateElements();

    int i1, i2, i3, i4, i5;
    ASSERT_TRUE (it.hasNext());

    ASSERT_TRUE (it.getNext(i1));
    ASSERT_EQ (1, i1);
    ASSERT_TRUE (it.getNext(i2));
    ASSERT_EQ (2, i2);
    ASSERT_TRUE (it.getNext(i3));
    ASSERT_EQ (3, i3);
    ASSERT_TRUE (it.getNext(i4));
    ASSERT_EQ (4, i4);
    ASSERT_TRUE (it.getNext(i5));
    ASSERT_EQ (5, i5);

    ASSERT_FALSE (it.hasNext());
}

TEST_F(Rope, IterateRopeElementsWithConcatOfTwoLeafNodes)
{
    tempo_utils::Rope<int> rope1{1, 2, 3};
    tempo_utils::Rope<int> rope2{4, 5};
    auto rope = rope1.append(rope2);

    auto it = rope.iterateElements();

    int i1, i2, i3, i4, i5;
    ASSERT_TRUE (it.hasNext());

    ASSERT_TRUE (it.getNext(i1));
    ASSERT_EQ (1, i1);
    ASSERT_TRUE (it.getNext(i2));
    ASSERT_EQ (2, i2);
    ASSERT_TRUE (it.getNext(i3));
    ASSERT_EQ (3, i3);
    ASSERT_TRUE (it.getNext(i4));
    ASSERT_EQ (4, i4);
    ASSERT_TRUE (it.getNext(i5));
    ASSERT_EQ (5, i5);

    ASSERT_FALSE (it.hasNext());
}

TEST_F(Rope, IterateRopeElementsWithConcatOfTwoConcatfNodes)
{
    tempo_utils::Rope<int> rope1{1, 2};
    tempo_utils::Rope<int> rope2{3};
    tempo_utils::Rope<int> rope3{4};
    tempo_utils::Rope<int> rope4{5};
    auto concat1 = rope1.append(rope2);
    auto concat2 = rope3.append(rope4);
    auto rope = concat1.append(concat2);

    auto it = rope.iterateElements();

    int i1, i2, i3, i4, i5;
    ASSERT_TRUE (it.hasNext());

    ASSERT_TRUE (it.getNext(i1));
    ASSERT_EQ (1, i1);
    ASSERT_TRUE (it.getNext(i2));
    ASSERT_EQ (2, i2);
    ASSERT_TRUE (it.getNext(i3));
    ASSERT_EQ (3, i3);
    ASSERT_TRUE (it.getNext(i4));
    ASSERT_EQ (4, i4);
    ASSERT_TRUE (it.getNext(i5));
    ASSERT_EQ (5, i5);

    ASSERT_FALSE (it.hasNext());
}
