
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <absl/strings/str_split.h>

#include <tempo_schema/schema_namespace.h>
#include <tempo_test/status_matchers.h>
#include <tempo_utils/unicode.h>

tempo_test::StatusMatcher::StatusMatcher()
    : m_type(MatcherType::INVALID)
{
}

tempo_test::StatusMatcher::StatusMatcher(tempo_utils::StatusCode statusCode)
    : m_type(MatcherType::STATUS_CODE),
      m_statusCode(statusCode)
{
}

tempo_test::StatusMatcher::StatusMatcher(std::string_view errorCategory)
    : m_type(MatcherType::ERROR_CATEGORY),
      m_errorCategory(errorCategory)
{
}

tempo_test::StatusMatcher::StatusMatcher(std::string_view errorCategory, int errorCode)
    : m_type(MatcherType::ERROR_CATEGORY_AND_CODE),
      m_errorCategory(errorCategory),
      m_errorCode(errorCode)
{
}

tempo_test::StatusMatcher::StatusMatcher(const StatusMatcher &other)
    : m_type(other.m_type),
      m_statusCode(other.m_statusCode),
      m_errorCategory(other.m_errorCategory),
      m_errorCode(other.m_errorCode)
{
}

bool
tempo_test::StatusMatcher::MatchAndExplain(
    const tempo_utils::Status &status,
    std::ostream* os) const
{
    switch (m_type) {
        case MatcherType::STATUS_CODE: {
            return status.getStatusCode() == m_statusCode;
        }
        case MatcherType::ERROR_CATEGORY: {
            return status.getErrorCategory() == m_errorCategory;
        }
        case MatcherType::ERROR_CATEGORY_AND_CODE: {
            return status.getErrorCategory() == m_errorCategory && status.getErrorCode() == m_errorCode;
        }
        default:
            return false;
    }
}

void
tempo_test::StatusMatcher::DescribeTo(std::ostream* os) const
{
    switch (m_type) {
        case MatcherType::STATUS_CODE:
            *os << "status matches status code " << std::string_view(tempo_utils::status_code_to_string(m_statusCode));
            break;
        case MatcherType::ERROR_CATEGORY:
            *os << "status matches error category " << m_errorCategory;
            break;
        case MatcherType::ERROR_CATEGORY_AND_CODE:
            *os << "status contains error category " << m_errorCategory << " and code " << m_errorCode;
            break;
        default:
            *os << "!!! invalid matcher !!!";
    }
}

void
tempo_test::StatusMatcher::DescribeNegationTo(std::ostream* os) const
{
    *os << "status does not match";
}

::testing::Matcher<tempo_utils::Status>
tempo_test::IsOk()
{
    return StatusMatcher(tempo_utils::StatusCode::kOk);
}

::testing::Matcher<tempo_utils::Status>
tempo_test::MatchesStatusCode(tempo_utils::StatusCode statusCode)
{
    return StatusMatcher(statusCode);
}

::testing::Matcher<tempo_utils::Status>
tempo_test::MatchesErrorCategory(std::string_view errorCategory)
{
    return StatusMatcher(errorCategory);
}

::testing::Matcher<tempo_utils::Status>
tempo_test::MatchesErrorCategory(const tempo_schema::SchemaNs &errorCategory)
{
    return StatusMatcher(errorCategory.getNs());
}

void
tempo_utils::PrintTo(const Status &status, std::ostream *os)
{
    *os << status.toString();
}