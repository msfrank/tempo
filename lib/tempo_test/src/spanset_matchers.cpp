
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <absl/strings/str_split.h>

#include <tempo_test/spanset_matchers.h>
#include <tempo_tracing/error_walker.h>
#include <tempo_tracing/tracing_schema.h>
#include <tempo_utils/unicode.h>

tempo_test::SpansetMatcher::SpansetMatcher()
{
}

tempo_test::SpansetMatcher::SpansetMatcher(const tempo_utils::Status &status)
    : m_status(status)
{
}

tempo_test::SpansetMatcher::SpansetMatcher(const SpansetMatcher &other)
    : m_status(other.m_status)
{
}

bool
tempo_test::SpansetMatcher::MatchAndExplain(
    const tempo_tracing::TempoSpanset &spanset,
    std::ostream* os) const
{
    auto errorWalker = spanset.getErrors();

    for (int i = 0; i < errorWalker.numErrors(); i++) {
        auto spanWalker = errorWalker.getError(i);
        for (int j = 0; j < spanWalker.numLogs(); j++) {
            auto logWalker = spanWalker.getLog(j);
            if (logWalker.getSeverity() != tempo_tracing::LogSeverity::kError)
                continue;
            std::string errorCategory;
            logWalker.parseField(tempo_tracing::kTempoTracingErrorCategoryName, errorCategory);
            tu_int64 errorCode;
            logWalker.parseField(tempo_tracing::kTempoTracingErrorCode, errorCode);

            if (m_status.getErrorCategory() == errorCategory && m_status.getErrorCode() == errorCode)
                return true;
        }
    }

    return false;
}

void
tempo_test::SpansetMatcher::DescribeTo(std::ostream* os) const
{
    *os << "spanset contains status " << m_status.toString();
}

void
tempo_test::SpansetMatcher::DescribeNegationTo(std::ostream* os) const
{
    *os << "spanset does not contain status " << m_status.toString();
}

std::ostream&
tempo_test::operator<<(std::ostream& os, const tempo_tracing::TempoSpanset &spanset)
{
    return os << "TempoSpanset";
}

void
tempo_test::PrintTo(const tempo_tracing::TempoSpanset &spanset, std::ostream *os)
{
    *os << "TempoSpanset";
}
