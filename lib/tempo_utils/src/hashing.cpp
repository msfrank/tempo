
#include <boost/container_hash/hash.hpp>

#include <tempo_utils/hashing.h>

size_t
tempo_utils::hash_combine(size_t seed, size_t value)
{
    boost::hash_combine(seed, value);
    return seed;
}
