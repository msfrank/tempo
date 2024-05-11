#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <tempo_allocator/memory_pool.h>

TEST(MemoryPool, TestAllocate)
{
    tempo_allocator::MemoryPool pool;

    struct Struct {
        int a;
        int b;
    };
    Struct *s = nullptr;

    s = (Struct *) pool.allocate(sizeof(Struct));
    ASSERT_TRUE (s);
    *s = {1, 2};
    pool.deallocate(s);

    s = pool.allocateInstance<Struct>();
    ASSERT_TRUE (s);
    *s = {1, 2};
    pool.deallocate(s);
}

TEST(MemoryPool, TestAllocateContiguous)
{
    tempo_allocator::MemoryPool pool;

    struct Struct {
        int a;
        int b;
    };
    Struct *s = nullptr;

    s = (Struct *) pool.allocateContiguous(2, sizeof(Struct));
    ASSERT_TRUE (s);
    s[0] = {1, 2};
    s[1] = {3, 4};
    pool.deallocate(s);

    s = pool.allocateArray<Struct>(2);
    ASSERT_TRUE (s);
    s[0] = {1, 2};
    s[1] = {3, 4};
    pool.deallocate(s);
}

TEST(MemoryPool, TestAllocateString)
{
    tempo_allocator::MemoryPool pool;

    tempo_allocator::ManagedString s;
    ASSERT_TRUE (s.empty());

    s = pool.newString("hello, world!");
    ASSERT_EQ (std::string("hello, world!"), s);
}

TEST(MemoryPool, TestAllocateVector)
{
    tempo_allocator::MemoryPool pool;

    tempo_allocator::ManagedVector<int> v;
    ASSERT_TRUE (v.empty());

    v = pool.newVector<int>();
    ASSERT_TRUE (v.empty());

    std::array<int,3> ints = {1,2,3};
    v = pool.newVector<int>(ints.begin(), ints.end());
    ASSERT_EQ (3, v.size());
    ASSERT_EQ (1, v[0]);
    ASSERT_EQ (2, v[1]);
    ASSERT_EQ (3, v[2]);
}

TEST(MemoryPool, TestAllocateMap)
{
    tempo_allocator::MemoryPool pool;

    tempo_allocator::ManagedMap<tempo_allocator::ManagedString,int> m;
    ASSERT_TRUE (m.empty());

    m = pool.newMap<tempo_allocator::ManagedString,int>();
    ASSERT_TRUE (m.empty());

    m = pool.newMap<tempo_allocator::ManagedString,int>({
        {pool.newString("foo"), 1},
        {pool.newString("bar"), 2},
        {pool.newString("baz"), 3},
    });
    ASSERT_EQ (3, m.size());
    ASSERT_EQ (2, m.at(pool.newString("bar")));
}

TEST(MemoryPool, TestAllocateSet)
{
    tempo_allocator::MemoryPool pool;

    tempo_allocator::ManagedSet<int> s;
    ASSERT_TRUE (s.empty());

    s = pool.newSet<int>();
    ASSERT_TRUE (s.empty());

    s = pool.newSet<int>({1, 2, 3});
    ASSERT_EQ (3, s.size());
    ASSERT_TRUE (s.contains(3));
    ASSERT_FALSE (s.contains(7));
}
