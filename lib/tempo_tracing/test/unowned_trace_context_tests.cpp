#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_tracing/internal/thread_context.h>
#include <tempo_tracing/span_walker.h>
#include <tempo_tracing/trace_context.h>
#include <tempo_tracing/trace_recorder.h>
#include <tempo_test/result_matchers.h>
#include <tempo_test/status_matchers.h>

#include "base_tracing_fixture.h"

class UnownedTraceContext : public BaseTracingFixture {
protected:
    void TearDown() override {
        tempo_tracing::internal::reset_thread_context();
    }
};

TEST_F(UnownedTraceContext, MakeUnownedContext)
{
    auto makeUnownedContextResult = tempo_tracing::TraceContext::makeUnownedContext(
        tempo_tracing::TraceRecorder::create(), "test");
    ASSERT_THAT (makeUnownedContextResult, tempo_test::IsResult());
    auto currentContext = tempo_tracing::TraceContext::currentContext();
    ASSERT_TRUE (currentContext == nullptr);
    auto testContext = tempo_tracing::TraceContext::getContext("test");
    ASSERT_TRUE (testContext != nullptr);
    ASSERT_EQ ("test", testContext->getName());
    ASSERT_TRUE (testContext->isUnowned());
}

TEST_F(UnownedTraceContext, MakeUnownedContextAndSwitch)
{
    auto makeUnownedContextResult = tempo_tracing::TraceContext::makeUnownedContextAndSwitch(
        tempo_tracing::TraceRecorder::create(), "test");
    ASSERT_THAT (makeUnownedContextResult, tempo_test::IsResult());
    auto currentContext = tempo_tracing::TraceContext::currentContext();
    auto testContext = tempo_tracing::TraceContext::getContext("test");
    ASSERT_EQ (currentContext, testContext);
    ASSERT_EQ ("test", currentContext->getName());
    ASSERT_TRUE (currentContext->isUnowned());
}

TEST_F(UnownedTraceContext, MakeUnownedContextThenSwitch)
{
    auto makeUnownedContextResult = tempo_tracing::TraceContext::makeUnownedContext(
        tempo_tracing::TraceRecorder::create(), "test");
    ASSERT_THAT (makeUnownedContextResult, tempo_test::IsResult());
    auto currentContext = tempo_tracing::TraceContext::currentContext();
    ASSERT_TRUE (currentContext == nullptr);
    auto switchCurrentResult = tempo_tracing::TraceContext::switchCurrent("test");
    ASSERT_THAT (switchCurrentResult, tempo_test::IsResult());
    auto testContext = switchCurrentResult.getResult();
    ASSERT_EQ ("test", testContext->getName());
    currentContext = tempo_tracing::TraceContext::currentContext();
    ASSERT_EQ (currentContext, testContext);
    ASSERT_EQ ("test", currentContext->getName());
    ASSERT_TRUE (currentContext->isUnowned());
}

TEST_F(UnownedTraceContext, FinishFailsForUnownedContext)
{
    auto makeUnownedContextResult = tempo_tracing::TraceContext::makeUnownedContext(
        tempo_tracing::TraceRecorder::create(), "test");
    ASSERT_THAT (makeUnownedContextResult, tempo_test::IsResult());
    auto context = makeUnownedContextResult.getResult();
    ASSERT_TRUE (context->isUnowned());

    auto span = context->pushSpan(
        tempo_tracing::FailurePropagation::NoPropagation, tempo_tracing::FailureCollection::IgnoresPropagation);
    span->setOperationName("TestOperation");

    auto finishContextResult = tempo_tracing::TraceContext::finishContext("test");
    ASSERT_THAT (finishContextResult, tempo_test::IsStatus());
}

TEST_F(UnownedTraceContext, ReleaseUnownedContext)
{
    auto makeUnownedContextResult = tempo_tracing::TraceContext::makeUnownedContext(
        tempo_tracing::TraceRecorder::create(), "test");
    ASSERT_THAT (makeUnownedContextResult, tempo_test::IsResult());
    auto context = makeUnownedContextResult.getResult();
    ASSERT_TRUE (context->isUnowned());

    auto span = context->pushSpan(
        tempo_tracing::FailurePropagation::NoPropagation, tempo_tracing::FailureCollection::IgnoresPropagation);
    span->setOperationName("TestOperation");
    auto traceId = context->getTraceId();

    auto releaseContextResult = tempo_tracing::TraceContext::releaseContext("test");
    ASSERT_THAT (releaseContextResult, tempo_test::IsResult());
    ASSERT_FALSE (tempo_tracing::TraceContext::hasContext("test"));

    auto recorder = releaseContextResult.getResult();
    ASSERT_EQ (traceId, recorder->traceId());
    ASSERT_FALSE (recorder->isClosed());
}
