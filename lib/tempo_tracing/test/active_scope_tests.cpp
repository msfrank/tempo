// #include <gtest/gtest.h>
//
// #include <tempo_tracing/active_scope.h>
// #include <tempo_tracing/trace_recorder.h>
//
// #include "base_tracing_fixture.h"
//
// class ActiveScope : public BaseTracingFixture {};
//
// TEST_F(ActiveScope, TestCreateSpanAndActivate)
// {
//     recorder = tempo_tracing::TraceRecorder::create();
//     auto span = recorder->makeSpan();
//
//     {
//         tempo_tracing::ActiveScope scope(span);
//     }
// }