#ifndef TEMPO_TEST_RESULT_MATCHERS_H
#define TEMPO_TEST_RESULT_MATCHERS_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_utils/result.h>

#include "status_matchers.h"

namespace tempo_test {

    template<typename MatchesType>
    class ResultMatcher {

    public:
        explicit ResultMatcher(const ::testing::Matcher<MatchesType> &matcher)
            : m_matcher(matcher) {};

        bool MatchAndExplain(const tempo_utils::Result<MatchesType> &result, std::ostream *os) const {
            if (result.isStatus())
                return false;
            return m_matcher.Matches(result.getResult());
        }
        void DescribeTo(std::ostream *os) const {
            m_matcher.DescribeTo(os);
        }
        void DescribeNegationTo(std::ostream *os) const {
            m_matcher.DescribeNegationTo(os);
        }

        using is_gtest_matcher = void;

    private:
        ::testing::Matcher<MatchesType> m_matcher;
    };

    template<typename MatchesType>
    ::testing::Matcher<tempo_utils::Result<MatchesType>> ContainsResult(const ::testing::Matcher<MatchesType> &matcher)
    {
        return ResultMatcher<MatchesType>(matcher);
    }

    template<typename MatchesType>
    class MaybeStatusMatcher {

    public:
        MaybeStatusMatcher() : m_isStatus(false) {};
        explicit MaybeStatusMatcher(tempo_utils::StatusCode statusCode)
            : m_matcher(statusCode) {};
        explicit MaybeStatusMatcher(std::string_view errorCategory)
            : m_matcher(errorCategory) {};
        MaybeStatusMatcher(std::string_view errorCategory, int errorCode)
            : m_matcher(errorCategory, errorCode) {};

        bool MatchAndExplain(const tempo_utils::MaybeStatus<MatchesType> &result, std::ostream *os) const {
            if (!m_isStatus) {
                return !result.isStatus();
            }
            if (!result.isStatus())
                return false;
            return m_matcher.MatchAndExplain(result.getStatus(), os);
        }
        void DescribeTo(std::ostream *os) const {
            if (m_isStatus) {
                m_matcher.DescribeTo(os);
            } else {
                *os << "no status is present";
            }
        }
        void DescribeNegationTo(std::ostream *os) const {
            if (m_isStatus) {
                m_matcher.DescribeNegationTo(os);
            } else {
                *os << "status is present";
            }
        }

        using is_gtest_matcher = void;

    private:
        bool m_isStatus = true;
        StatusMatcher m_matcher;
    };

    ::testing::Matcher<tempo_utils::MaybeStatus<tempo_utils::Status>> IsStatus();
    ::testing::Matcher<tempo_utils::MaybeStatus<tempo_utils::Status>> IsResult();

    template<typename ConditionType,
        typename StatusType = typename tempo_utils::ConditionTraits<ConditionType>::StatusType>
    ::testing::Matcher<tempo_utils::MaybeStatus<tempo_utils::Status>> ContainsStatus(ConditionType condition)
    {
        return MaybeStatusMatcher<tempo_utils::Status>(
            tempo_utils::ConditionTraits<ConditionType>::condition_namespace(),
            static_cast<int>(condition));
    }
}

namespace tempo_utils {

    template<class T>
    void PrintTo(const MaybeStatus<T> &maybeStatus, std::ostream *os)
    {
        if (maybeStatus.isStatus()) {
            ::testing::internal::UniversalPrint(maybeStatus.getStatus(), os);
            //PrintTo(maybeStatus.getStatus(), os);
        } else {
            *os << "no status";
        }
    }

    template<class T>
    void PrintTo(const Result<T> &result, std::ostream *os)
    {
        if (result.isResult()) {
            ::testing::internal::UniversalPrint(result.getResult(), os);
            //PrintTo(result.getResult(), os);
        } else {
            PrintTo(result.getStatus(), os);
        }
    }
}

#endif // TEMPO_TEST_RESULT_MATCHERS_H