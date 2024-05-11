
#include <tempo_allocator/allocator_types.h>

bool
tempo_allocator::operator==(const ManagedString &lhs, const std::string &rhs)
{
    return std::string_view(lhs.data(), lhs.size()) == std::string_view(rhs.data(), rhs.size());
}

bool
tempo_allocator::operator==(const std::string &lhs, const ManagedString &rhs)
{
    return std::string_view(lhs.data(), lhs.size()) == std::string_view(rhs.data(), rhs.size());
}