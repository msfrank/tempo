#ifndef TEMPO_ALLOCATOR_ARENA_POOL_H
#define TEMPO_ALLOCATOR_ARENA_POOL_H

#include <tempo_utils/result.h>

#include "memory_pool.h"

namespace tempo_allocator {

    enum class AutoReleaseMode {
        Manual,
        AutoRelease,
    };

    class ArenaPool : public MemoryPool {

    public:
        static tempo_utils::Result<std::shared_ptr<ArenaPool>> create(
            std::string_view name,
            AutoReleaseMode autoRelease = AutoReleaseMode::Manual);
        ~ArenaPool();

        std::string getName() const;

        tempo_utils::Status decay();
        tempo_utils::Status purge();
        tempo_utils::Status reset();
        tempo_utils::Status destroy();

    private:
        std::string m_name;
        AutoReleaseMode m_autoRelease;
        absl::Mutex m_lock;
        bool m_destroyed;

        ArenaPool(tu_uint32 arena, std::string_view name, AutoReleaseMode autoRelease);
    };
}


#endif // TEMPO_ALLOCATOR_ARENA_POOL_H