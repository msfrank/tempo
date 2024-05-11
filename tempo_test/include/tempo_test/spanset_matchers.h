#ifndef TEMPO_TEST_SPANSET_MATCHERS_H
#define TEMPO_TEST_SPANSET_MATCHERS_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_tracing/tempo_spanset.h>
#include <tempo_utils/status.h>

namespace tempo_test {

    class SpansetMatcher {

    public:
        SpansetMatcher();
        explicit SpansetMatcher(const tempo_utils::Status &status);
        SpansetMatcher(const SpansetMatcher &other);

        bool MatchAndExplain(const tempo_tracing::TempoSpanset &spanset, std::ostream *os) const;
        void DescribeTo(std::ostream *os) const;
        void DescribeNegationTo(std::ostream *os) const;

        using MatchesType = tempo_tracing::TempoSpanset;
        using is_gtest_matcher = void;

    private:
        tempo_utils::Status m_status;
    };

    template<typename ConditionType,
        typename StatusType = typename tempo_utils::ConditionTraits<ConditionType>::StatusType>
    ::testing::Matcher<tempo_tracing::TempoSpanset> SpansetContainsError(ConditionType conditionType)
    {
        auto status = StatusType::forCondition(conditionType);
        return SpansetMatcher(status);
    }

    void PrintTo(const tempo_tracing::TempoSpanset &spanset, std::ostream *os);
    std::ostream& operator<<(std::ostream& os, const tempo_tracing::TempoSpanset &spanset);
}

#endif // TEMPO_TEST_SPANSET_MATCHERS_H