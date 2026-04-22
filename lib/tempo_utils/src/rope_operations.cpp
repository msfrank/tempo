
#include <boost/math/special_functions/fibonacci.hpp>

#include <tempo_utils/rope_operations.h>

bool
tempo_utils::RopeOperations::isBalanced() const
{
    auto depth = getDepth();
    auto size = getSize();
    return boost::math::fibonacci<size_t>(depth + 2) <= size;
}
