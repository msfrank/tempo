#include <gtest/gtest.h>

#include <tempo_tracing/leaf_scope.h>
#include <tempo_tracing/internal/thread_context.h>
#include <tempo_tracing/trace_recorder.h>

#include "base_tracing_fixture.h"

class LeafScope : public BaseTracingFixture {
protected:
    void TearDown() override {
        tempo_tracing::internal::reset_thread_context();
    }
};

TEST_F(LeafScope, PushSpanOnCurrentContext)
{
    tempo_tracing::TraceContext::makeContextAndSwitch("test");
    ASSERT_EQ ("test", tempo_tracing::TraceContext::currentContext()->getName());

    {
        tempo_tracing::LeafScope scope("TestOperation");
        auto span = scope.getSpan();

        auto currentContext = tempo_tracing::TraceContext::currentContext();
        ASSERT_TRUE (currentContext->isActive());
        ASSERT_EQ (1, currentContext->numSpans());
        auto top = currentContext->peekSpan();
        ASSERT_EQ (top->spanId(), span->spanId());
        ASSERT_EQ ("TestOperation", span->getOperationName());
    }
}

TEST_F(LeafScope, PushSpanOnSpecifiedContext)
{
    tempo_tracing::TraceContext::makeContext("test");
    ASSERT_TRUE (tempo_tracing::TraceContext::currentContext() == nullptr);

    {
        tempo_tracing::LeafScope scope("TestOperation", "test");
        auto span = scope.getSpan();

        auto currentContext = tempo_tracing::TraceContext::currentContext();
        ASSERT_TRUE (currentContext->isActive());
        ASSERT_EQ (1, currentContext->numSpans());
        auto top = currentContext->peekSpan();
        ASSERT_EQ (top->spanId(), span->spanId());
        ASSERT_EQ ("TestOperation", span->getOperationName());
        ASSERT_TRUE (span->isActive());
    }
}

TEST_F(LeafScope, PopSpanOnCurrentContextAfterScopeDestructs)
{
    tempo_tracing::TraceContext::makeContextAndSwitch("test");
    ASSERT_EQ ("test", tempo_tracing::TraceContext::currentContext()->getName());

    {
        tempo_tracing::LeafScope scope("TestOperation");
        auto span = scope.getSpan();

        auto currentContext = tempo_tracing::TraceContext::currentContext();
        ASSERT_TRUE (currentContext->isActive());
        ASSERT_EQ (1, currentContext->numSpans());
        auto top = currentContext->peekSpan();
        ASSERT_EQ (top->spanId(), span->spanId());
        ASSERT_EQ ("TestOperation", span->getOperationName());
    }

    auto currentContext = tempo_tracing::TraceContext::currentContext();
    ASSERT_TRUE (currentContext->isActive());
    ASSERT_TRUE (currentContext->isEmpty());
}

TEST_F(LeafScope, PopSpanOnSpecifiedContextAfterScopeDestructs)
{
    tempo_tracing::TraceContext::makeContext("test");
    ASSERT_TRUE (tempo_tracing::TraceContext::currentContext() == nullptr);

    {
        tempo_tracing::LeafScope scope("TestOperation", "test");
        auto span = scope.getSpan();

        auto currentContext = tempo_tracing::TraceContext::currentContext();
        ASSERT_TRUE (currentContext->isActive());
        ASSERT_EQ (1, currentContext->numSpans());
        auto top = currentContext->peekSpan();
        ASSERT_EQ (top->spanId(), span->spanId());
        ASSERT_EQ ("TestOperation", span->getOperationName());
    }

    auto currentContext = tempo_tracing::TraceContext::currentContext();
    ASSERT_TRUE (currentContext->isActive());
    ASSERT_TRUE (currentContext->isEmpty());
}

TEST_F(LeafScope, ScopeConstructionFailsIfNoCurrentContext)
{
    tempo_tracing::LeafScope scope("TestOperation");
    ASSERT_FALSE (scope.isValid());
    auto span = scope.getSpan();
    ASSERT_TRUE (span == nullptr);
}

TEST_F(LeafScope, ScopeConstructionFailsIfNoSpecifiedContext)
{
    tempo_tracing::LeafScope scope("TestOperation", "test");
    ASSERT_FALSE (scope.isValid());
    auto span = scope.getSpan();
    ASSERT_TRUE (span == nullptr);
}
