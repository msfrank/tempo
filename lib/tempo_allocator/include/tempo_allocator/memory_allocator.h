#ifndef TEMPO_ALLOCATOR_MEMORY_ALLOCATOR_H
#define TEMPO_ALLOCATOR_MEMORY_ALLOCATOR_H

#include <cstddef>
#include <limits>

#include <tempo_utils/integer_types.h>

namespace tempo_allocator {

    namespace internal {
        constexpr tu_uint32 kDefaultArena = 0xffffffff;
        void *allocateMemory(std::size_t size, tu_uint32 arena);
        void deallocateMemory(void *ptr);
    }

    template<typename T>
    struct RemoveConst
    {
        typedef T value_type;
    };

    template<typename T>
    struct RemoveConst<const T>
    {
        typedef T value_type;
    };

    template <class T>
    class MemoryAllocator {

    public:
        typedef RemoveConst<T>              Base;
        typedef typename Base::value_type   value_type;
        typedef value_type*                 pointer;
        typedef const value_type*           const_pointer;
        typedef value_type&                 reference;
        typedef const value_type&           const_reference;
        typedef std::size_t                 size_type;
        typedef std::ptrdiff_t              difference_type;

        /**
         * Rebind allocator to type U.
         *
         * @tparam U
         */
        template <class U>
        struct rebind {
            typedef MemoryAllocator<U> other;
        };

    private:
        /**
         * Construct a MemoryAllocator with the specified arena. This constructor is private and is only meant to
         * be used from within a MemoryPool.
         *
         * @param arena The arena index.
         */
        MemoryAllocator(tu_uint32 arena) noexcept
            : m_arena(arena)
        {
        }
        friend class MemoryPool;

    public:
        /**
         * Construct a MemoryAllocator with the default arena.
         *
         */
        MemoryAllocator() noexcept
            : m_arena(internal::kDefaultArena)
        {
        }

        MemoryAllocator(const MemoryAllocator &other) noexcept
            : m_arena(other.m_arena)
        {
        }

        template <class U>
        MemoryAllocator(const MemoryAllocator<U> &other) noexcept
            : m_arena(other.m_arena)
        {
        }

        ~MemoryAllocator() noexcept
        {
        }

        /**
         * Return address of values.
         *
         * @param value
         * @return
         */
        pointer
        address(reference value) const
        {
            return &value;
        }

        /**
         *
         * @param value
         * @return
         */
        const_pointer
        address(const_reference value) const
        {
            return &value;
        }

        /**
         * Return maximum number of elements that can be allocated.
         *
         * @return
         */
        size_type
        max_size() const throw()
        {
            return std::numeric_limits<std::size_t>::max() / sizeof(T);
        }

        /**
         * Allocate but don't initialize num elements of type T.
         *
         * @param num
         * @return
         */
        pointer
        allocate(size_type num, const void* = 0)
        {
            return (pointer) internal::allocateMemory(num * sizeof(T), m_arena);
        }

        /**
         * Initialize elements of allocated storage p with value value.
         *
         * @param p
         * @param value
         */
        void
        construct(pointer p, const T& value)
        {
            // initialize memory with placement new
            new((void*) p) T(value);
        }

        /**
         * Destroy elements of initialized storage p.
         *
         * @param p
         */
        void
        destroy(pointer p)
        {
            // destroy objects by calling their destructor
            p->~T();
        }

        /**
         * Deallocate storage p of deleted elements.
         *
         * @param p
         * @param num
         */
        void
        deallocate(pointer p, size_type num)
        {
            internal::deallocateMemory(p);
        }

        const tu_uint32 m_arena;
    };

    /**
     * All specializations of this allocator are interchangeable.
     *
     * @tparam T1
     * @tparam T2
     * @return
     */
    template <class T1, class T2>
    bool operator==(const MemoryAllocator<T1> &lhs, const MemoryAllocator<T2> &rhs) noexcept
    {
        //return lhs.getArena() != internal::kDefaultArena && rhs.getArena() != internal::kDefaultArena;
        return true;
    }

    /**
     * All specializations of this allocator are interchangeable.
     *
     * @tparam T1
     * @tparam T2
     * @return
     */
    template <class T1, class T2>
    bool operator!=(const MemoryAllocator<T1> &lhs, const MemoryAllocator<T2> &rhs) noexcept
    {
        //return lhs.getArena() == internal::kDefaultArena || rhs.getArena() == internal::kDefaultArena;
        return false;
    }
}


#endif // TEMPO_ALLOCATOR_MEMORY_ALLOCATOR_H
