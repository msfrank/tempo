
#include <jemalloc/jemalloc.h>

#include <tempo_allocator/arena_pool.h>
#include <tempo_utils/posix_result.h>

tempo_allocator::ArenaPool::ArenaPool(tu_uint32 arena, std::string_view name, AutoReleaseMode autoRelease)
    : MemoryPool(arena),
      m_name(name),
      m_autoRelease(autoRelease),
      m_destroyed(false)
{
}

tempo_utils::Result<std::shared_ptr<tempo_allocator::ArenaPool>>
tempo_allocator::ArenaPool::create(
    std::string_view name,
    AutoReleaseMode autoRelease)
{
    tu_uint32 arena = -1;
    size_t arena_sz = sizeof(arena);

    auto ret = je_mallctl(
        "arenas.create",
        (void *) &arena,
        &arena_sz,
        nullptr,
        sizeof(extent_hooks_t *));
    if (ret < 0)
        return tempo_utils::PosixStatus::fromError(ret, "failed to create memory arena");

    return std::shared_ptr<ArenaPool>(new ArenaPool(arena, name, autoRelease));
}

tempo_allocator::ArenaPool::~ArenaPool()
{
    if (m_autoRelease == AutoReleaseMode::AutoRelease)
        destroy();
    absl::MutexLock locker(&m_lock);
    TU_LOG_FATAL_IF(!m_destroyed) << "attempting to deallocate active arena";
}

tempo_utils::Status
tempo_allocator::ArenaPool::decay()
{
    absl::MutexLock locker(&m_lock);
    if (m_destroyed)
        return tempo_utils::PosixStatus::ok();

    auto mib = absl::StrCat("arena.", getArena(), ".decay");
    auto ret = je_mallctl(
        mib.c_str(),
        nullptr,
        nullptr,
        nullptr,
        0);
    if (ret < 0)
        return tempo_utils::PosixStatus::fromError(ret, "failed to decay memory arena");

    return tempo_utils::PosixStatus::ok();
}

std::string
tempo_allocator::ArenaPool::getName() const
{
    return m_name;
}

tempo_utils::Status
tempo_allocator::ArenaPool::purge()
{
    absl::MutexLock locker(&m_lock);
    if (m_destroyed)
        return tempo_utils::PosixStatus::ok();

    auto mib = absl::StrCat("arena.", getArena(), ".purge");
    auto ret = je_mallctl(
        mib.c_str(),
        nullptr,
        nullptr,
        nullptr,
        0);
    if (ret < 0)
        return tempo_utils::PosixStatus::fromError(ret, "failed to purge memory arena");

    return tempo_utils::PosixStatus::ok();
}

tempo_utils::Status
tempo_allocator::ArenaPool::reset()
{
    absl::MutexLock locker(&m_lock);
    if (m_destroyed)
        return tempo_utils::PosixStatus::ok();

    auto mib = absl::StrCat("arena.", getArena(), ".reset");
    auto ret = je_mallctl(
        mib.c_str(),
        nullptr,
        nullptr,
        nullptr,
        0);
    if (ret < 0)
        return tempo_utils::PosixStatus::fromError(ret, "failed to reset memory arena");

    return tempo_utils::PosixStatus::ok();
}

tempo_utils::Status
tempo_allocator::ArenaPool::destroy()
{
    absl::MutexLock locker(&m_lock);
    if (m_destroyed)
        return tempo_utils::PosixStatus::ok();

    auto mib = absl::StrCat("arena.", getArena(), ".destroy");
    auto ret = je_mallctl(
        mib.c_str(),
        nullptr,
        nullptr,
        nullptr,
        0);
    if (ret < 0)
        return tempo_utils::PosixStatus::fromError(ret, "failed to destroy memory arena");

    m_destroyed = true;
    return tempo_utils::PosixStatus::ok();
}