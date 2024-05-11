#include <gtest/gtest.h>

#include <tempo_tracing/internal/spanset_reader.h>
#include <tempo_tracing/span_walker.h>
#include <tempo_tracing/trace_recorder.h>
#include <tempo_tracing/tracing_schema.h>

TEST(StatusSpan, TestCreateStatusSpan)
{
    auto recorder = tempo_tracing::TraceRecorder::create();
    auto span = recorder->makeSpan();
    auto status = span->close(
        tempo_utils::GenericCondition::kInternalViolation,
        tempo_tracing::LogSeverity::kError);

    ASSERT_EQ (tempo_utils::GenericCondition::kInternalViolation, status.getCondition());

    recorder->close();
    auto toSpansetResult = recorder->toSpanset();
    ASSERT_TRUE (toSpansetResult.isResult());
    auto spanset = toSpansetResult.getResult();
    ASSERT_EQ (status.getTraceId(), spanset.getTraceId());
    auto reader = spanset.getReader();
    ASSERT_EQ (1, reader->numSpans());

    auto spanWalker = spanset.getRoots().getRoot(0);
    ASSERT_TRUE (spanWalker.isValid());
    ASSERT_EQ (span->spanId(), spanWalker.getId());

    std::string event;
    ASSERT_TRUE (spanWalker.parseTag(tempo_tracing::kOpentracingEvent, event).isOk());
    ASSERT_EQ ("error", event);
//
//    std::string message;
//    ASSERT_TRUE (spanWalker.parseTag(tempo_tracing::kOpentracingMessage, message).isOk());
//    ASSERT_TRUE (message.empty());
}

TEST(StatusSpan, TestCreateStatusSpanWithMessage)
{
    auto recorder = tempo_tracing::TraceRecorder::create();
    auto span = recorder->makeSpan();
    auto status = span->close(
        tempo_utils::GenericCondition::kInternalViolation,
        tempo_tracing::LogSeverity::kError,
        "test");

    ASSERT_EQ (tempo_utils::GenericCondition::kInternalViolation, status.getCondition());

    recorder->close();
    auto toSpansetResult = recorder->toSpanset();
    ASSERT_TRUE (toSpansetResult.isResult());
    auto spanset = toSpansetResult.getResult();
    ASSERT_EQ (status.getTraceId(), spanset.getTraceId());
    auto reader = spanset.getReader();
    ASSERT_EQ (1, reader->numSpans());

    auto spanWalker = spanset.getRoots().getRoot(0);
    ASSERT_TRUE (spanWalker.isValid());
    ASSERT_EQ (span->spanId(), spanWalker.getId());

    std::string event;
    ASSERT_TRUE (spanWalker.parseTag(tempo_tracing::kOpentracingEvent, event).isOk());
    ASSERT_EQ ("error", event);

    ASSERT_EQ (1, spanWalker.numLogs());
    auto logWalker = spanWalker.getLog(0);

    std::string errorCategory;
    ASSERT_TRUE (logWalker.parseField(tempo_tracing::kTempoTracingErrorCategoryName, errorCategory).isOk());
    ASSERT_EQ (tempo_utils::kTempoUtilsGenericStatusNs.getNs(), errorCategory);

    tu_int64 errorCode;
    ASSERT_TRUE (logWalker.parseField(tempo_tracing::kTempoTracingErrorCode, errorCode).isOk());
    ASSERT_EQ (static_cast<tu_int64>(tempo_utils::GenericCondition::kInternalViolation), errorCode);

    std::string message;
    ASSERT_TRUE (logWalker.parseField(tempo_tracing::kOpentracingMessage, message).isOk());
    ASSERT_EQ ("test", message);
}

TEST(StatusSpan, TestCreateStatusSpanWithFormattedMessage)
{
    auto recorder = tempo_tracing::TraceRecorder::create();
    auto span = recorder->makeSpan();
    auto status = span->close(
        tempo_utils::GenericCondition::kInternalViolation,
        tempo_tracing::LogSeverity::kError,
        "status is {}", "ok");

    ASSERT_EQ (tempo_utils::GenericCondition::kInternalViolation, status.getCondition());

    recorder->close();
    auto toSpansetResult = recorder->toSpanset();
    ASSERT_TRUE (toSpansetResult.isResult());
    auto spanset = toSpansetResult.getResult();
    ASSERT_EQ (status.getTraceId(), spanset.getTraceId());
    auto reader = spanset.getReader();
    ASSERT_EQ (1, reader->numSpans());

    auto spanWalker = spanset.getRoots().getRoot(0);
    ASSERT_TRUE (spanWalker.isValid());
    ASSERT_EQ (span->spanId(), spanWalker.getId());

    std::string event;
    ASSERT_TRUE (spanWalker.parseTag(tempo_tracing::kOpentracingEvent, event).isOk());
    ASSERT_EQ ("error", event);


    ASSERT_EQ (1, spanWalker.numLogs());
    auto logWalker = spanWalker.getLog(0);

    std::string errorCategory;
    ASSERT_TRUE (logWalker.parseField(tempo_tracing::kTempoTracingErrorCategoryName, errorCategory).isOk());
    ASSERT_EQ (tempo_utils::kTempoUtilsGenericStatusNs.getNs(), errorCategory);

    tu_int64 errorCode;
    ASSERT_TRUE (logWalker.parseField(tempo_tracing::kTempoTracingErrorCode, errorCode).isOk());
    ASSERT_EQ (static_cast<tu_int64>(tempo_utils::GenericCondition::kInternalViolation), errorCode);

    std::string message;
    ASSERT_TRUE (logWalker.parseField(tempo_tracing::kOpentracingMessage, message).isOk());
    ASSERT_EQ ("status is ok", message);
}
