#include <gtest/gtest.h>

#include <tempo_tracing/active_scope.h>
#include <tempo_tracing/trace_recorder.h>
#include <tempo_tracing/tracing_schema.h>

TEST(ActiveScope, TestCreateSpanAndActivate)
{
    auto recorder = tempo_tracing::TraceRecorder::create();
    auto span = recorder->makeSpan();

    {
        tempo_tracing::ActiveScope scope(span);
    }
}