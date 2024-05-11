#include <gtest/gtest.h>

#include <tempo_test/spanset_matchers.h>
#include <tempo_tracing/trace_recorder.h>

using namespace tempo_test;

TEST(SpansetMatcher, MatchesOkStatus)
{
    auto recorder = tempo_tracing::TraceRecorder::create();
    auto span = recorder->makeSpan();
    auto status = span->close(
        tempo_utils::GenericCondition::kInternalViolation,
        tempo_tracing::LogSeverity::kError);
    recorder->close();
    auto spanset = recorder->toSpanset().orElseThrow();

    ASSERT_THAT (spanset, SpansetContainsError(tempo_utils::GenericCondition::kInternalViolation));
}
