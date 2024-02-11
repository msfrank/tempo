
#include <jemalloc/jemalloc.h>

#include <tempo_allocator/memory_pool.h>
#include <tempo_utils/log_message.h>

tempo_allocator::MemoryPool::MemoryPool(tu_uint32 arena)
    : m_arena(arena)
{
}

tempo_allocator::MemoryPool::MemoryPool()
    : MemoryPool(internal::kDefaultArena)
{
}

tu_uint32
tempo_allocator::MemoryPool::getArena() const
{
    return m_arena;
}

/**
 * Allocate size bytes of memory in the MemoryPool.
 *
 * @param size The number of bytes to allocate.
 * @return A pointer to the allocated memory, or nullptr if memory could not be allocated.
 */
void *
tempo_allocator::MemoryPool::allocate(std::size_t size)
{
    MemoryAllocator<tu_int8> alloc;
    return alloc.allocate(size);
}

/**
 * Allocate a contiguous chunk of memory for nmembers objects of size bytes in the MemoryPool.
 *
 * @param nmembers The number of objects to allocate.
 * @param size The size of an object in bytes.
 * @return A pointer to the allocated memory, or nullptr if memory could not be allocated.
 */
void *
tempo_allocator::MemoryPool::allocateContiguous(std::size_t nmembers, std::size_t size)
{
    MemoryAllocator<tu_int8> alloc;
    return alloc.allocate(size * nmembers);
}

/**
 * Frees the memory referenced by the specified pointer.
 *
 * @param ptr A pointer to the memory to free.
 */
void
tempo_allocator::MemoryPool::deallocate(void *ptr)
{
    MemoryAllocator<tu_int8> alloc;
    alloc.deallocate((MemoryAllocator<tu_int8>::pointer) ptr, 1);
}

/**
 * Returns a newly constructed ManagedString with the contents of view, allocated with memory from the
 * MemoryPool. This depends on copy-elision to efficiently return by value.
 *
 * @param view The contents to initialize the ManagedString with.
 * @return The ManagedString.
 */
tempo_allocator::ManagedString
tempo_allocator::MemoryPool::newString(std::string_view view)
{
    return std::basic_string<char,std::char_traits<char>,MemoryAllocator<char>>
        (view.data(), view.size(), MemoryAllocator<char>{m_arena});
}
