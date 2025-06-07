#ifndef BASE_TRACING_FIXTURE_H
#define BASE_TRACING_FIXTURE_H

#include <gtest/gtest.h>

#include <tempo_tracing/trace_recorder.h>

class BaseTracingFixture : public ::testing::Test {
protected:
    std::shared_ptr<tempo_tracing::TraceRecorder> recorder;
    void SetUp() override;
};

#endif //BASE_TRACING_FIXTURE_H
