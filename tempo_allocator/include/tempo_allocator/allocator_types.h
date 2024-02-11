#ifndef TEMPO_ALLOCATOR_ALLOCATOR_TYPES_H
#define TEMPO_ALLOCATOR_ALLOCATOR_TYPES_H

#include <string>

#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>

#include "memory_allocator.h"

namespace tempo_allocator {

    using ManagedString = std::basic_string<char,std::char_traits<char>,MemoryAllocator<char>>;

    bool operator==(const ManagedString &lhs, const std::string &rhs);
    bool operator==(const std::string &lhs, const ManagedString &rhs);

    template <class T>
    using ManagedVector = std::vector<T,MemoryAllocator<T>>;

    template <class K, class V, typename HashType = absl::Hash<K>, typename EqType = std::equal_to<K>>
    using ManagedMap = absl::flat_hash_map<K, V, HashType, EqType, MemoryAllocator<std::pair<const K,V>>>;

    template <class E, typename HashType = absl::Hash<E>, typename EqType = std::equal_to<E>>
    using ManagedSet = absl::flat_hash_set<E, HashType, EqType, MemoryAllocator<E>>;
}

#endif // TEMPO_ALLOCATOR_ALLOCATOR_TYPES_H
