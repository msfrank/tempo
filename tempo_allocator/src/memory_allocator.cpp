
#include <jemalloc/jemalloc.h>

#include <tempo_allocator/memory_allocator.h>

void *
tempo_allocator::internal::allocateMemory(std::size_t size, tu_uint32 arena)
{
    if (arena == 0xffffffff)
        return je_malloc(size);

    return je_mallocx(size, MALLOCX_ARENA(arena));
}

void
tempo_allocator::internal::deallocateMemory(void *ptr)
{
    return je_free(ptr);
}
