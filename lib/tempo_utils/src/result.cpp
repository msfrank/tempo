
#include <tempo_utils/result.h>

tempo_utils::IsStatus::IsStatus(bool isStatus)
    : m_isStatus(isStatus)
{
}

tempo_utils::IsStatus::IsStatus(const IsStatus &other)
    : m_isStatus(other.m_isStatus)
{
}

tempo_utils::IsStatus::IsStatus(IsStatus &&other) noexcept
{
    m_isStatus = other.m_isStatus;
}

tempo_utils::IsStatus&
tempo_utils::IsStatus::operator=(const IsStatus &other)
{
    if (this != &other) {
        m_isStatus = other.m_isStatus;
    }
    return *this;
}

tempo_utils::IsStatus&
tempo_utils::IsStatus::operator=(IsStatus &&other) noexcept
{
    if (this != &other) {
        m_isStatus = other.m_isStatus;
    }
    return *this;
}

bool
tempo_utils::IsStatus::isStatus() const
{
    return m_isStatus;
}