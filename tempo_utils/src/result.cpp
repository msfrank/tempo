
#include <tempo_utils/result.h>

tempo_utils::IsStatus::IsStatus(bool isStatus)
    : m_isStatus(isStatus)
{
}

bool
tempo_utils::IsStatus::isStatus() const
{
    return m_isStatus;
}