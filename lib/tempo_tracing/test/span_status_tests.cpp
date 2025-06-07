#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_tracing/internal/spanset_reader.h>
#include <tempo_tracing/span_walker.h>
#include <tempo_tracing/trace_recorder.h>
#include <tempo_tracing/tracing_schema.h>

#include "base_tracing_fixture.h"
#include "tempo_test/result_matchers.h"
#include "tempo_test/status_matchers.h"

class SpanStatus : public BaseTracingFixture {};

TEST_F(SpanStatus, CreateSpanAndCloseWithStatus)
{
    auto span = recorder->makeSpan();
    auto status = span->close(
        tempo_utils::GenericCondition::kInternalViolation,
        tempo_tracing::LogSeverity::kError);

    ASSERT_THAT (status, tempo_test::IsCondition(tempo_utils::GenericCondition::kInternalViolation));

    recorder->close();
    auto toSpansetResult = recorder->toSpanset();
    ASSERT_THAT (toSpansetResult, tempo_test::IsResult());
    auto spanset = toSpansetResult.getResult();
    ASSERT_EQ (status.getTraceId(), spanset.getTraceId());

    auto reader = spanset.getReader();
    ASSERT_EQ (1, reader->numSpans());

    auto spanWalker = spanset.getRoots().getRoot(0);
    ASSERT_TRUE (spanWalker.isValid());
    ASSERT_EQ (span->spanId(), spanWalker.getId());

    // span is failed
    ASSERT_TRUE (spanWalker.isFailed());

    // span has event tag
    std::string event;
    ASSERT_TRUE (spanWalker.parseTag(tempo_tracing::kOpentracingEvent, event).isOk());
    ASSERT_EQ ("error", event);

    // span has 1 log
    ASSERT_EQ (1, spanWalker.numLogs());
    auto logWalker = spanWalker.getLog(0);
    ASSERT_TRUE (logWalker.isValid());

    // log has error category name field
    std::string errorCategory;
    ASSERT_TRUE (logWalker.parseField(tempo_tracing::kTempoTracingErrorCategoryName, errorCategory).isOk());
    ASSERT_EQ (tempo_utils::ConditionTraits<tempo_utils::GenericCondition>::condition_namespace(),
        errorCategory);

    // log has error code field
    tu_int64 errorCode;
    ASSERT_TRUE (logWalker.parseField(tempo_tracing::kTempoTracingErrorCode, errorCode).isOk());
    ASSERT_EQ (tempo_utils::GenericCondition::kInternalViolation,
        static_cast<tempo_utils::GenericCondition>(errorCode));
}

TEST_F(SpanStatus, CreateSpanAndCloseWithStatusAndMessage)
{
    auto span = recorder->makeSpan();
    auto status = span->close(
        tempo_utils::GenericCondition::kInternalViolation,
        tempo_tracing::LogSeverity::kError,
        "test");

    ASSERT_THAT (status, tempo_test::IsCondition(tempo_utils::GenericCondition::kInternalViolation));

    recorder->close();
    auto toSpansetResult = recorder->toSpanset();
    ASSERT_THAT (toSpansetResult, tempo_test::IsResult());
    auto spanset = toSpansetResult.getResult();
    ASSERT_EQ (status.getTraceId(), spanset.getTraceId());

    auto reader = spanset.getReader();
    ASSERT_EQ (1, reader->numSpans());

    auto spanWalker = spanset.getRoots().getRoot(0);
    ASSERT_TRUE (spanWalker.isValid());
    ASSERT_EQ (span->spanId(), spanWalker.getId());

    // span is failed
    ASSERT_TRUE (spanWalker.isFailed());

    // span has event tag
    std::string event;
    ASSERT_TRUE (spanWalker.parseTag(tempo_tracing::kOpentracingEvent, event).isOk());
    ASSERT_EQ ("error", event);

    // span has 1 log
    ASSERT_EQ (1, spanWalker.numLogs());
    auto logWalker = spanWalker.getLog(0);
    ASSERT_TRUE (logWalker.isValid());

    // log has error category name field
    std::string errorCategory;
    ASSERT_TRUE (logWalker.parseField(tempo_tracing::kTempoTracingErrorCategoryName, errorCategory).isOk());
    ASSERT_EQ (tempo_utils::ConditionTraits<tempo_utils::GenericCondition>::condition_namespace(),
        errorCategory);

    // log has error code field
    tu_int64 errorCode;
    ASSERT_TRUE (logWalker.parseField(tempo_tracing::kTempoTracingErrorCode, errorCode).isOk());
    ASSERT_EQ (tempo_utils::GenericCondition::kInternalViolation,
        static_cast<tempo_utils::GenericCondition>(errorCode));

    // log has message field
    std::string message;
    ASSERT_TRUE (logWalker.parseField(tempo_tracing::kOpentracingMessage, message).isOk());
    ASSERT_EQ ("test", message);
}

TEST_F(SpanStatus, CreateSpanAndCloseWithStatusAndFormattedMessage)
{
    recorder = tempo_tracing::TraceRecorder::create();
    auto span = recorder->makeSpan();
    auto status = span->close(
        tempo_utils::GenericCondition::kInternalViolation,
        tempo_tracing::LogSeverity::kError,
        "status is {}", "failed");

    ASSERT_THAT (status, tempo_test::IsCondition(tempo_utils::GenericCondition::kInternalViolation));

    recorder->close();
    auto toSpansetResult = recorder->toSpanset();
    ASSERT_THAT (toSpansetResult, tempo_test::IsResult());
    auto spanset = toSpansetResult.getResult();
    ASSERT_EQ (status.getTraceId(), spanset.getTraceId());

    auto reader = spanset.getReader();
    ASSERT_EQ (1, reader->numSpans());

    auto spanWalker = spanset.getRoots().getRoot(0);
    ASSERT_TRUE (spanWalker.isValid());
    ASSERT_EQ (span->spanId(), spanWalker.getId());

    // span is failed
    ASSERT_TRUE (spanWalker.isFailed());

    // span has event tag
    std::string event;
    ASSERT_TRUE (spanWalker.parseTag(tempo_tracing::kOpentracingEvent, event).isOk());
    ASSERT_EQ ("error", event);

    // span has 1 log
    ASSERT_EQ (1, spanWalker.numLogs());
    auto logWalker = spanWalker.getLog(0);
    ASSERT_TRUE (logWalker.isValid());

    // log has error category name field
    std::string errorCategory;
    ASSERT_TRUE (logWalker.parseField(tempo_tracing::kTempoTracingErrorCategoryName, errorCategory).isOk());
    ASSERT_EQ (tempo_utils::ConditionTraits<tempo_utils::GenericCondition>::condition_namespace(),
        errorCategory);

    // log has error code field
    tu_int64 errorCode;
    ASSERT_TRUE (logWalker.parseField(tempo_tracing::kTempoTracingErrorCode, errorCode).isOk());
    ASSERT_EQ (tempo_utils::GenericCondition::kInternalViolation,
        static_cast<tempo_utils::GenericCondition>(errorCode));

    // log has message field
    std::string message;
    ASSERT_TRUE (logWalker.parseField(tempo_tracing::kOpentracingMessage, message).isOk());
    ASSERT_EQ ("status is failed", message);
}
