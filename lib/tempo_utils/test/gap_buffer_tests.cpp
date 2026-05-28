
#include <absl/strings/str_cat.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <tempo_utils/gap_buffer.h>

class GapBuffer : public ::testing::Test {};

TEST_F(GapBuffer, ConstructEmpty)
{
    tempo_utils::GapBuffer<char> buf;

    ASSERT_TRUE (buf.isEmpty());
    ASSERT_EQ (0, buf.numElements());
    ASSERT_EQ (0, buf.gapSize());
    ASSERT_EQ (0, buf.getCursor());
    ASSERT_EQ (0, buf.getCursorEnd());
}

TEST_F(GapBuffer, ConstructEmptyWithSize)
{
    tempo_utils::GapBuffer<char> buf(64);

    ASSERT_TRUE (buf.isEmpty());
    ASSERT_EQ (0, buf.numElements());
    ASSERT_EQ (64, buf.gapSize());
    ASSERT_EQ (0, buf.getCursor());
    ASSERT_EQ (buf.bufferSize(), buf.getCursorEnd());
}

TEST_F(GapBuffer, ConstructFromPointerAndSize)
{
    std::string initial = "hello, world!";
    tempo_utils::GapBuffer<char> buf(initial.data(), initial.size());

    ASSERT_FALSE (buf.isEmpty());
    ASSERT_EQ (initial.size(), buf.numElements());
    ASSERT_EQ (0, buf.gapSize());
    ASSERT_EQ (initial.size(), buf.getCursor());
    ASSERT_EQ (buf.bufferSize(), buf.getCursorEnd());
}

TEST_F(GapBuffer, ConstructFromSpan)
{
    std::string initial = "hello, world!";
    tempo_utils::GapBuffer<char> buf(std::span(initial.data(), initial.size()));

    ASSERT_FALSE (buf.isEmpty());
    ASSERT_EQ (initial.size(), buf.numElements());
    ASSERT_EQ (0, buf.gapSize());
    ASSERT_EQ (initial.size(), buf.getCursor());
    ASSERT_EQ (buf.bufferSize(), buf.getCursorEnd());
}

TEST_F(GapBuffer, ConstructFromIterator)
{
    std::string initial = "hello, world!";
    tempo_utils::GapBuffer<char> buf(initial.cbegin(), initial.cend());

    ASSERT_FALSE (buf.isEmpty());
    ASSERT_EQ (initial.size(), buf.numElements());
    ASSERT_EQ (0, buf.gapSize());
    ASSERT_EQ (initial.size(), buf.getCursor());
    ASSERT_EQ (buf.bufferSize(), buf.getCursorEnd());
}

TEST_F(GapBuffer, MoveToStart)
{
    std::string initial = "hello, world!";
    tempo_utils::GapBuffer<char> buf(initial);

    buf.moveToStart();
    ASSERT_EQ (buf.bufferSize() - 13, buf.gapSize());
    ASSERT_EQ (0, buf.getCursor());
    ASSERT_EQ (0, buf.getCursorEnd());
    ASSERT_EQ ('h', buf.cursorElement());
}

TEST_F(GapBuffer, MoveToMiddle)
{
    std::string initial = "hello, world!";
    tempo_utils::GapBuffer<char> buf(initial);

    buf.moveBefore(5);
    ASSERT_EQ (buf.bufferSize() - 13, buf.gapSize());
    ASSERT_EQ (5, buf.getCursor());
    ASSERT_EQ (5, buf.getCursorEnd());
    ASSERT_EQ ('o', buf.cursorElement());
}

TEST_F(GapBuffer, MoveToEnd)
{
    std::string initial = "hello, world!";
    tempo_utils::GapBuffer<char> buf(initial);
    buf.moveToStart();

    buf.moveToEnd();
    ASSERT_EQ (buf.bufferSize() - 13, buf.gapSize());
    ASSERT_EQ (13, buf.getCursor());
    ASSERT_EQ (13, buf.getCursorEnd());
    ASSERT_EQ ('!', buf.cursorElement());
}

TEST_F(GapBuffer, InsertIntoEmpty)
{
    tempo_utils::GapBuffer<char> buf;

    char ch = 'a';
    buf.insert(ch);
    ASSERT_FALSE (buf.isEmpty());
    ASSERT_EQ (1, buf.numElements());
    ASSERT_EQ (buf.bufferSize() - 1, buf.gapSize());
    ASSERT_EQ (1, buf.getCursor());
    ASSERT_EQ (buf.bufferSize(), buf.getCursorEnd());
    ASSERT_EQ (ch, buf.cursorElement());
}

TEST_F(GapBuffer, InsertAtStart)
{
    std::string initial = "ello, world!";
    tempo_utils::GapBuffer<char> buf(initial);
    buf.moveToStart();

    char ch = 'h';
    buf.insert(ch);
    ASSERT_FALSE (buf.isEmpty());
    ASSERT_EQ (13, buf.numElements());
    ASSERT_EQ (buf.bufferSize() - 13, buf.gapSize());
    ASSERT_EQ (1, buf.getCursor());
    ASSERT_EQ (buf.getCursor() + buf.gapSize(), buf.getCursorEnd());
    ASSERT_EQ (ch, buf.cursorElement());

    auto vec = buf.toVector();
    std::string s(vec.data(), vec.size());
    ASSERT_EQ ("hello, world!", s);
}

TEST_F(GapBuffer, InsertAtMiddle)
{
    std::string initial = "hello world!";
    tempo_utils::GapBuffer<char> buf(initial);
    buf.moveAfter(4);

    char ch = ',';
    buf.insert(ch);
    ASSERT_FALSE (buf.isEmpty());
    ASSERT_EQ (13, buf.numElements());
    ASSERT_EQ (buf.bufferSize() - 13, buf.gapSize());
    ASSERT_EQ (6, buf.getCursor());
    ASSERT_EQ (buf.getCursor() + buf.gapSize(), buf.getCursorEnd());
    ASSERT_EQ (ch, buf.cursorElement());

    auto vec = buf.toVector();
    std::string s(vec.data(), vec.size());
    ASSERT_EQ ("hello, world!", s);
}

TEST_F(GapBuffer, InsertAtEnd)
{
    std::string initial = "hello, world";
    tempo_utils::GapBuffer<char> buf(initial);

    char ch = '!';
    buf.insert(ch);
    ASSERT_FALSE (buf.isEmpty());
    ASSERT_EQ (13, buf.numElements());
    ASSERT_EQ (buf.bufferSize() - 13, buf.gapSize());
    ASSERT_EQ (13, buf.getCursor());
    ASSERT_EQ (buf.bufferSize(), buf.getCursorEnd());
    ASSERT_EQ (ch, buf.cursorElement());

    auto vec = buf.toVector();
    std::string s(vec.data(), vec.size());
    ASSERT_EQ ("hello, world!", s);
}

TEST_F(GapBuffer, InsertRange)
{
    std::string initial = "hello!";
    tempo_utils::GapBuffer<char> buf(initial);
    buf.stepBackward();

    std::string elements = ", world";
    buf.insert(elements.cbegin(), elements.cend());
    ASSERT_FALSE (buf.isEmpty());
    ASSERT_EQ (13, buf.numElements());
    ASSERT_EQ (buf.bufferSize() - 13, buf.gapSize());
    ASSERT_EQ (12, buf.getCursor());
    ASSERT_EQ (buf.bufferSize() - 1, buf.getCursorEnd());

    auto vec = buf.toVector();
    std::string s(vec.data(), vec.size());
    ASSERT_EQ ("hello, world!", s);
}

TEST_F(GapBuffer, RemoveForward)
{
    std::string initial = "hello, world!";
    tempo_utils::GapBuffer<char> buf(initial);
    buf.moveAfter(4);

    buf.removeForward(7);
    ASSERT_FALSE (buf.isEmpty());
    ASSERT_EQ (6, buf.numElements());
    ASSERT_EQ (buf.bufferSize() - 6, buf.gapSize());
    ASSERT_EQ (5, buf.getCursor());

    auto vec = buf.toVector();
    std::string s(vec.data(), vec.size());
    ASSERT_EQ ("hello!", s);
}

TEST_F(GapBuffer, RemoveBackward)
{
    std::string initial = "hello, world!";
    tempo_utils::GapBuffer<char> buf(initial);
    buf.moveBefore(12);

    buf.removeBackward(7);
    ASSERT_FALSE (buf.isEmpty());
    ASSERT_EQ (6, buf.numElements());
    ASSERT_EQ (buf.bufferSize() - 6, buf.gapSize());
    ASSERT_EQ (5, buf.getCursor());

    auto vec = buf.toVector();
    std::string s(vec.data(), vec.size());
    ASSERT_EQ ("hello!", s);
}
