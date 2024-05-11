#ifndef TEMPO_TEST_STATUS_MATCHERS_H
#define TEMPO_TEST_STATUS_MATCHERS_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_utils/status.h>

namespace tempo_test {

    class StatusMatcher {

        enum class MatcherType {
            INVALID,
            ERROR_CATEGORY,
            ERROR_CATEGORY_AND_CODE,
            STATUS_CODE,
        };

    public:
        StatusMatcher();
        explicit StatusMatcher(tempo_utils::StatusCode statusCode);
        explicit StatusMatcher(std::string_view errorCategory);
        explicit StatusMatcher(std::string_view errorCategory, int errorCode);
        StatusMatcher(const StatusMatcher &other);

        bool MatchAndExplain(const tempo_utils::Status &status, std::ostream *os) const;
        void DescribeTo(std::ostream *os) const;
        void DescribeNegationTo(std::ostream *os) const;

        using is_gtest_matcher = void;

    private:
        MatcherType m_type;
        tempo_utils::StatusCode m_statusCode;
        std::string m_errorCategory;
        int m_errorCode;
    };

    template<typename ConditionType,
        typename StatusType = typename tempo_utils::ConditionTraits<ConditionType>::StatusType>
    ::testing::Matcher<tempo_utils::Status> IsCondition(ConditionType condition)
    {
        return StatusMatcher(
            tempo_utils::ConditionTraits<ConditionType>::condition_namespace(),
            static_cast<int>(condition));
    }

    ::testing::Matcher<tempo_utils::Status> MatchesStatusCode(tempo_utils::StatusCode statusCode);
    ::testing::Matcher<tempo_utils::Status> MatchesErrorCategory(const tempo_utils::SchemaNs &errorCategory);
    ::testing::Matcher<tempo_utils::Status> MatchesErrorCategory(std::string_view errorCategory);

    void PrintTo(const tempo_utils::Status &status, std::ostream *os);
    std::ostream& operator<<(std::ostream& os, const tempo_utils::Status &status);
}

#endif // TEMPO_TEST_STATUS_MATCHERS_H