#include <gtest/gtest.h>

#include <tempo_tracing/enter_scope.h>
#include <tempo_tracing/internal/thread_context.h>
#include <tempo_tracing/trace_recorder.h>

#include "base_tracing_fixture.h"

class EnterScope : public BaseTracingFixture {
protected:
    void TearDown() override {
        tempo_tracing::internal::reset_thread_context();
    }
};

TEST_F(EnterScope, PushSpanOnCurrentContext)
{
    tempo_tracing::TraceContext::makeContextAndSwitch("test");
    ASSERT_EQ ("test", tempo_tracing::TraceContext::currentContext()->getName());

    {
        tempo_tracing::EnterScope scope("TestOperation");
        auto span = scope.getSpan();

        auto *currentContext = tempo_tracing::TraceContext::currentContext();
        ASSERT_TRUE (currentContext->isActive());
        ASSERT_EQ (1, currentContext->numSpans());
        auto top = currentContext->peekSpan();
        ASSERT_EQ (top->spanId(), span->spanId());
        ASSERT_EQ ("TestOperation", span->getOperationName());
    }
}

TEST_F(EnterScope, PushSpanOnSpecifiedContext)
{
    tempo_tracing::TraceContext::makeContext("test");
    ASSERT_TRUE (tempo_tracing::TraceContext::currentContext() == nullptr);

    {
        tempo_tracing::EnterScope scope("TestOperation", "test");
        auto span = scope.getSpan();

        auto *currentContext = tempo_tracing::TraceContext::currentContext();
        ASSERT_TRUE (currentContext->isActive());
        ASSERT_EQ (1, currentContext->numSpans());
        auto top = currentContext->peekSpan();
        ASSERT_EQ (top->spanId(), span->spanId());
        ASSERT_EQ ("TestOperation", span->getOperationName());
        ASSERT_TRUE (span->isActive());
    }
}

TEST_F(EnterScope, SpanRemainsOnStackAfterScopeDestructs)
{
    tempo_tracing::TraceContext::makeContext("test");
    ASSERT_TRUE (tempo_tracing::TraceContext::currentContext() == nullptr);

    tempo_utils::SpanId spanId;
    {
        tempo_tracing::EnterScope scope("TestOperation", "test");
        spanId = scope.getSpan()->spanId();
    }

    auto *currentContext = tempo_tracing::TraceContext::currentContext();
    ASSERT_TRUE (currentContext->isActive());
    ASSERT_EQ (1, currentContext->numSpans());
    auto top = currentContext->peekSpan();
    ASSERT_EQ (top->spanId(), spanId);
    ASSERT_EQ ("TestOperation", top->getOperationName());
    ASSERT_FALSE (top->isActive());
}

TEST_F(EnterScope, ScopeConstructionFailsIfNoCurrentContext)
{
    tempo_tracing::EnterScope scope("TestOperation");
    ASSERT_FALSE (scope.isValid());
    auto span = scope.getSpan();
    ASSERT_TRUE (span == nullptr);
}

TEST_F(EnterScope, ScopeConstructionFailsIfNoSpecifiedContext)
{
    tempo_tracing::EnterScope scope("TestOperation", "test");
    ASSERT_FALSE (scope.isValid());
    auto span = scope.getSpan();
    ASSERT_TRUE (span == nullptr);
}
