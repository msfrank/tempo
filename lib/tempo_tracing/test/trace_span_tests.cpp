#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_tracing/internal/spanset_reader.h>
#include <tempo_tracing/span_walker.h>
#include <tempo_tracing/trace_recorder.h>
#include <tempo_tracing/tracing_schema.h>
#include <tempo_test/result_matchers.h>
#include <tempo_test/status_matchers.h>

TEST(TraceSpan, CheckStatusOk)
{
    auto recorder = tempo_tracing::TraceRecorder::create();

    auto span = recorder->makeSpan();
    auto status = span->checkStatus(tempo_utils::Status());
    span->close();

    ASSERT_THAT (status, tempo_test::IsOk());

    recorder->close();
    auto toSpansetResult = recorder->toSpanset();
    ASSERT_THAT (toSpansetResult, tempo_test::IsResult());
    auto spanset = toSpansetResult.getResult();

    auto reader = spanset.getReader();
    ASSERT_EQ (1, reader->numSpans());

    auto spanWalker = spanset.getRoots().getRoot(0);
    ASSERT_TRUE (spanWalker.isValid());
    ASSERT_EQ (span->spanId(), spanWalker.getId());

    ASSERT_EQ (0, spanWalker.numLogs());
}

TEST(TraceSpan, CheckStatusNotOk)
{
    auto recorder = tempo_tracing::TraceRecorder::create();

    auto span = recorder->makeSpan();
    auto status = span->checkStatus(tempo_utils::GenericStatus::forCondition(
        tempo_utils::GenericCondition::kInternalViolation, "foobar"));
    span->close();

    ASSERT_THAT (status, tempo_test::IsCondition(tempo_utils::GenericCondition::kInternalViolation));

    recorder->close();
    auto toSpansetResult = recorder->toSpanset();
    ASSERT_THAT (toSpansetResult, tempo_test::IsResult());
    auto spanset = toSpansetResult.getResult();

    auto reader = spanset.getReader();
    ASSERT_EQ (1, reader->numSpans());

    auto spanWalker = spanset.getRoots().getRoot(0);
    ASSERT_TRUE (spanWalker.isValid());
    ASSERT_EQ (span->spanId(), spanWalker.getId());

    ASSERT_EQ (1, spanWalker.numLogs());
    auto logWalker = spanWalker.getLog(0);

    std::string message;
    ASSERT_TRUE (logWalker.parseField(tempo_tracing::kOpentracingMessage, message).isOk());
    ASSERT_EQ ("foobar", message);
}

TEST(TraceSpan, CheckResultOk)
{
    auto recorder = tempo_tracing::TraceRecorder::create();

    auto span = recorder->makeSpan();
    auto result = span->checkResult(tempo_utils::Result<std::string>(std::string{"foobar"}));
    span->close();

    ASSERT_THAT (result, tempo_test::IsResult());

    recorder->close();
    auto toSpansetResult = recorder->toSpanset();
    ASSERT_THAT (toSpansetResult, tempo_test::IsResult());
    auto spanset = toSpansetResult.getResult();

    auto reader = spanset.getReader();
    ASSERT_EQ (1, reader->numSpans());

    auto spanWalker = spanset.getRoots().getRoot(0);
    ASSERT_TRUE (spanWalker.isValid());
    ASSERT_EQ (span->spanId(), spanWalker.getId());

    ASSERT_EQ (0, spanWalker.numLogs());
}

TEST(TraceSpan, CheckResultNotOk)
{
    auto recorder = tempo_tracing::TraceRecorder::create();

    auto span = recorder->makeSpan();
    auto result = span->checkResult(tempo_utils::Result<std::string>(
        tempo_utils::GenericStatus::forCondition(
            tempo_utils::GenericCondition::kInternalViolation, "foobar")));
    span->close();

    ASSERT_THAT (result, tempo_test::ContainsStatus(tempo_utils::GenericCondition::kInternalViolation));

    recorder->close();
    auto toSpansetResult = recorder->toSpanset();
    ASSERT_THAT (toSpansetResult, tempo_test::IsResult());
    auto spanset = toSpansetResult.getResult();

    auto reader = spanset.getReader();
    ASSERT_EQ (1, reader->numSpans());

    auto spanWalker = spanset.getRoots().getRoot(0);
    ASSERT_TRUE (spanWalker.isValid());
    ASSERT_EQ (span->spanId(), spanWalker.getId());

    ASSERT_EQ (1, spanWalker.numLogs());
    auto logWalker = spanWalker.getLog(0);

    std::string message;
    ASSERT_TRUE (logWalker.parseField(tempo_tracing::kOpentracingMessage, message).isOk());
    ASSERT_EQ ("foobar", message);
}
