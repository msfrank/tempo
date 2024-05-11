#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <tempo_allocator/arena_pool.h>

TEST(ArenaPool, TestCreateAndDestroy)
{
    auto createArenaResult = tempo_allocator::ArenaPool::create("test");
    ASSERT_TRUE (createArenaResult.isResult());
    auto arenaPool = createArenaResult.getResult();

    ASSERT_EQ ("test", arenaPool->getName());

    auto *ptr = arenaPool->allocate(16);
    ASSERT_TRUE (ptr != nullptr);
    arenaPool->deallocate(ptr);

    arenaPool->destroy();
    arenaPool.reset();
}

TEST(ArenaPool, TestDecay)
{
    auto createArenaResult = tempo_allocator::ArenaPool::create("test", tempo_allocator::AutoReleaseMode::AutoRelease);
    ASSERT_TRUE (createArenaResult.isResult());
    auto arenaPool = createArenaResult.getResult();

    auto *ptr = arenaPool->allocate(16);
    ASSERT_TRUE (ptr != nullptr);
    arenaPool->deallocate(ptr);

    arenaPool->decay();
    arenaPool.reset();
}

TEST(ArenaPool, TestPurge)
{
    auto createArenaResult = tempo_allocator::ArenaPool::create("test", tempo_allocator::AutoReleaseMode::AutoRelease);
    ASSERT_TRUE (createArenaResult.isResult());
    auto arenaPool = createArenaResult.getResult();

    ASSERT_EQ ("test", arenaPool->getName());

    auto *ptr = arenaPool->allocate(16);
    ASSERT_TRUE (ptr != nullptr);
    arenaPool->deallocate(ptr);

    arenaPool->purge();
    arenaPool.reset();
}

TEST(ArenaPool, TestReset)
{
    auto createArenaResult = tempo_allocator::ArenaPool::create("test", tempo_allocator::AutoReleaseMode::AutoRelease);
    ASSERT_TRUE (createArenaResult.isResult());
    auto arenaPool = createArenaResult.getResult();

    ASSERT_EQ ("test", arenaPool->getName());

    auto *ptr = arenaPool->allocate(16);
    ASSERT_TRUE (ptr != nullptr);
    arenaPool->deallocate(ptr);

    arenaPool->reset();
    arenaPool.reset();
}
