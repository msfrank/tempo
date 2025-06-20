#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_tracing/internal/thread_context.h>
#include <tempo_tracing/span_walker.h>
#include <tempo_tracing/trace_context.h>
#include <tempo_tracing/trace_recorder.h>
#include <tempo_test/result_matchers.h>
#include <tempo_test/status_matchers.h>

#include "base_tracing_fixture.h"

class TraceContext : public BaseTracingFixture {
protected:
    void TearDown() override {
        tempo_tracing::internal::reset_thread_context();
    }
};

TEST_F(TraceContext, ThreadInitiallyHasNoCurrentContext)
{
    ASSERT_EQ (nullptr, tempo_tracing::TraceContext::currentContext());
}

TEST_F(TraceContext, MakeContext)
{
    auto makeContextResult = tempo_tracing::TraceContext::makeContext("test");
    ASSERT_THAT (makeContextResult, tempo_test::IsResult());
    auto currentContext = tempo_tracing::TraceContext::currentContext();
    ASSERT_TRUE (currentContext == nullptr);
    auto testContext = tempo_tracing::TraceContext::getContext("test");
    ASSERT_TRUE (testContext != nullptr);
    ASSERT_EQ ("test", testContext->getName());
}

TEST_F(TraceContext, MakeContextAndSwitch)
{
    auto makeContextResult = tempo_tracing::TraceContext::makeContextAndSwitch("test");
    ASSERT_THAT (makeContextResult, tempo_test::IsResult());
    auto currentContext = tempo_tracing::TraceContext::currentContext();
    auto testContext = tempo_tracing::TraceContext::getContext("test");
    ASSERT_EQ (currentContext, testContext);
    ASSERT_EQ ("test", currentContext->getName());
}

TEST_F(TraceContext, MakeContextThenSwitch)
{
    auto makeContextResult = tempo_tracing::TraceContext::makeContext("test");
    ASSERT_THAT (makeContextResult, tempo_test::IsResult());
    auto currentContext = tempo_tracing::TraceContext::currentContext();
    ASSERT_TRUE (currentContext == nullptr);
    auto switchCurrentResult = tempo_tracing::TraceContext::switchCurrent("test");
    ASSERT_THAT (switchCurrentResult, tempo_test::IsResult());
    auto testContext = switchCurrentResult.getResult();
    ASSERT_EQ ("test", testContext->getName());
    currentContext = tempo_tracing::TraceContext::currentContext();
    ASSERT_EQ (currentContext, testContext);
    ASSERT_EQ ("test", currentContext->getName());
}

TEST_F(TraceContext, FinishContext)
{
    auto makeContextResult = tempo_tracing::TraceContext::makeContext("test");
    ASSERT_THAT (makeContextResult, tempo_test::IsResult());
    auto context = makeContextResult.getResult();
    auto span = context->pushSpan(
        tempo_tracing::FailurePropagation::NoPropagation, tempo_tracing::FailureCollection::IgnoresPropagation);
    span->setOperationName("TestOperation");
    auto traceId = context->getTraceId();

    auto finishContextResult = tempo_tracing::TraceContext::finishContext("test");
    ASSERT_THAT (finishContextResult, tempo_test::IsResult());
    ASSERT_FALSE (tempo_tracing::TraceContext::hasContext("test"));

    auto spanset = finishContextResult.getResult();
    ASSERT_TRUE (spanset.isValid());
    ASSERT_EQ (traceId, spanset.getTraceId());
}

TEST_F(TraceContext, ReleaseContext)
{
    auto makeContextResult = tempo_tracing::TraceContext::makeContext("test");
    ASSERT_THAT (makeContextResult, tempo_test::IsResult());
    auto context = makeContextResult.getResult();
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
