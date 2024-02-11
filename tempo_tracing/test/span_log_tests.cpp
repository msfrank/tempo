#include <gtest/gtest.h>

#include <tempo_tracing/internal/spanset_reader.h>
#include <tempo_tracing/span_log.h>
#include <tempo_tracing/span_walker.h>
#include <tempo_tracing/trace_recorder.h>
#include <tempo_tracing/tracing_schema.h>

TEST(SpanLog, TestCreateSpanWithLog)
{
    auto recorder = tempo_tracing::TraceRecorder::create();
    auto span = recorder->makeSpan();
    auto log = span->appendLog(absl::Now(), tempo_tracing::LogSeverity::kError);
    log->putField(tempo_tracing::kOpentracingMessage, std::string("test"));
    recorder->close();

    auto toSpansetResult = recorder->toSpanset();
    ASSERT_TRUE (toSpansetResult.isResult());
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
    //ASSERT_EQ ("test", message);
}