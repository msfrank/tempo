#ifndef TEMPO_ALLOCATOR_MEMORY_POOL_H
#define TEMPO_ALLOCATOR_MEMORY_POOL_H

#include <tempo_utils/integer_types.h>

#include "allocator_types.h"
#include "memory_allocator.h"

namespace tempo_allocator {

    /**
     *
     */
    class MemoryPool {
    public:
        MemoryPool();

        void *allocate(std::size_t size);
        void *allocateContiguous(std::size_t nmembers, std::size_t size);
        void deallocate(void *ptr);

        ManagedString newString(std::string_view view);

    protected:
        MemoryPool(tu_uint32 arena);
        tu_uint32 getArena() const;

    private:
        const tu_uint32 m_arena;

    public:
        /**
         *
         * @tparam T
         * @return
         */
        template <class T>
        T* allocateInstance()
        {
            return (T*) allocate(sizeof(T));
        }

        /**
         *
         * @tparam T
         * @param nmembers
         * @return
         */
        template <class T>
        T* allocateArray(std::size_t nmembers)
        {
            return (T*) allocateContiguous(nmembers, sizeof(T));
        }

        /**
         * Allocate a new vector of type T with the given size, allocated with memory from the MemoryPool.
         * This depends on copy-elision to efficiently return by value.
         *
         * @tparam T The element type.
         * @param size The size of the vector.
         * @return The ManagedVector.
         */
        template <class T>
        ManagedVector<T> newVector(size_t size = 0)
        {
            return std::vector<T,MemoryAllocator<T>>(size, MemoryAllocator<T>(m_arena));
        }

        /**
         * Allocate a new vector of type T with contents from the given range of [first, last), allocated with
         * memory from the MemoryPool. This depends on copy-elision to efficiently return by value.
         *
         * @tparam T The element type.
         * @param first An iterator pointing to the first element of the range.
         * @param last An iterator pointing to the end of the range.
         * @return The ManagedVector.
         */
        template <class T, class InputIt>
        ManagedVector<T> newVector(InputIt first, InputIt last)
        {
            return std::vector<T,MemoryAllocator<T>>(first, last, MemoryAllocator<T>(m_arena));
        }

        /**
         * Allocate a new map of key type K and value type V initialized with values from init, allocated with
         * memory from the MemoryPool. This depends on copy-elision to efficiently return by value.
         *
         * @tparam K The key type.
         * @tparam V The value type.
         * @tparam InputIt The input iterator type.
         * @tparam HashType The hash implementation for K.
         * @tparam EqType The equality implementation for K.
         * @tparam EntryType The (K,V) pair type.
         * @param init The list of entries to initialize the map with, defaults to empty.
         * @return The ManagedMap.
         */
        template <class K, class V,
            typename HashType = absl::Hash<K>,
            typename EqType = std::equal_to<K>,
            typename EntryType = typename ManagedMap<K,V,HashType,EqType>::value_type>
        ManagedMap<K,V,HashType,EqType> newMap(std::initializer_list<EntryType> init = {})
        {
            return absl::flat_hash_map<K,V,HashType,EqType,MemoryAllocator<EntryType>>
                (init, MemoryAllocator<EntryType>(m_arena));
        }

        /**
         * Allocate a new map of key type K and value type V initialized with contents from the given range
         * of [first, last), allocated with memory from the MemoryPool. This depends on copy-elision to
         * efficiently return by value.
         *
         * @tparam K The key type.
         * @tparam V The value type.
         * @tparam InputIt The input iterator type.
         * @tparam HashType The hash implementation for K.
         * @tparam EqType The equality implementation for K.
         * @tparam EntryType The (K,V) pair type.
         * @param first An iterator pointing to the first element of the range.
         * @param last An iterator pointing to the end of the range.
         * @return The ManagedMap.
         */
        template <class K, class V, class InputIt,
            typename HashType = absl::Hash<K>,
            typename EqType = std::equal_to<K>,
            typename EntryType = typename ManagedMap<K,V,HashType,EqType>::value_type>
        ManagedMap<K,V,HashType,EqType> newMap(InputIt first, InputIt last)
        {
            return absl::flat_hash_map<K,V,HashType,EqType,MemoryAllocator<EntryType>>
                (first, last, MemoryAllocator<EntryType>(m_arena));
        }

        /**
         * Allocate a new set of element type E initialized with values from init, allocated with memory from
         * the MemoryPool. This depends on copy-elision to efficiently return by value.
         *
         * @tparam E The element type.
         * @tparam HashType The hash implementation for E.
         * @tparam EqType The equality implementation for E.
         * @param init The list of elements to initialize the set with, defaults to empty.
         * @return The ManagedSet.
         */
        template <class E,
            typename HashType = absl::Hash<E>,
            typename EqType = std::equal_to<E>>
        ManagedSet<E,HashType,EqType> newSet(std::initializer_list<E> init = {})
        {
            return absl::flat_hash_set<E,HashType,EqType,MemoryAllocator<E>>
                (init, MemoryAllocator<E>(m_arena));
        }

        /**
         * Allocate a new set of element type E initialized with contents from the given range of [first, last),
         * allocated with memory from the MemoryPool. This depends on copy-elision to efficiently return by value.
         *
         * @tparam E The element type.
         * @tparam InputIt The input iterator type.
         * @tparam HashType The hash implementation for E.
         * @tparam EqType The equality implementation for E.
         * @param first An iterator pointing to the first element of the range.
         * @param last An iterator pointing to the end of the range.
         * @return The ManagedSet.
         */
        template <class E, class InputIt,
            typename HashType = absl::Hash<E>,
            typename EqType = std::equal_to<E>>
        ManagedSet<E,HashType,EqType> newSet(InputIt first, InputIt last)
        {
            return absl::flat_hash_set<E,HashType,EqType,MemoryAllocator<E>>
                (first, last, MemoryAllocator<E>(m_arena));
        }

        /**
         * Returns an allocator specialized for T which allocates from the memory pool.
         *
         * @tparam T The value type.
         * @return The allocator.
         */
        template <class T>
        MemoryAllocator<T> getAllocator()
        {
            return MemoryAllocator<T>(m_arena);
        }
    };
}

#endif // TEMPO_ALLOCATOR_MEMORY_POOL_H