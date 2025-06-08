#include <gtest/gtest.h>

#include <tempo_tracing/current_scope.h>
#include <tempo_tracing/enter_scope.h>
#include <tempo_tracing/exit_scope.h>
#include <tempo_tracing/internal/thread_context.h>
#include <tempo_tracing/trace_recorder.h>

#include "base_tracing_fixture.h"

class CurrentScope : public BaseTracingFixture {
protected:
    void TearDown() override {
        tempo_tracing::internal::reset_thread_context();
    }
};

TEST_F(CurrentScope, PeekSpanOnCurrentContext)
{
    tempo_tracing::TraceContext::makeContextAndSwitch("test");
    ASSERT_EQ ("test", tempo_tracing::TraceContext::currentContext()->getName());

    {
        tempo_tracing::EnterScope scope("TestOperation");
    }

    {
        tempo_tracing::CurrentScope scope;
        auto span = scope.getSpan();

        auto *currentContext = tempo_tracing::TraceContext::currentContext();
        ASSERT_TRUE (currentContext->isActive());
        ASSERT_EQ (1, currentContext->numSpans());
        auto top = currentContext->peekSpan();
        ASSERT_EQ (top->spanId(), span->spanId());
        ASSERT_EQ ("TestOperation", span->getOperationName());
    }
}

TEST_F(CurrentScope, PeekSpanOnSpecifiedContext)
{
    tempo_tracing::TraceContext::makeContext("test");
    ASSERT_TRUE (tempo_tracing::TraceContext::currentContext() == nullptr);

    {
        tempo_tracing::EnterScope scope("TestOperation", "test");
    }

    {
        tempo_tracing::CurrentScope scope("test");
        auto span = scope.getSpan();

        auto *currentContext = tempo_tracing::TraceContext::currentContext();
        ASSERT_TRUE (currentContext->isActive());
        ASSERT_EQ (1, currentContext->numSpans());
        auto top = currentContext->peekSpan();
        ASSERT_EQ (top->spanId(), span->spanId());
        ASSERT_EQ ("TestOperation", span->getOperationName());
    }
}

TEST_F(CurrentScope, SpanRemainsOnStackAfterScopeDestructs)
{
    tempo_tracing::TraceContext::makeContext("test");
    ASSERT_TRUE (tempo_tracing::TraceContext::currentContext() == nullptr);

    tempo_utils::SpanId spanId;
    {
        tempo_tracing::EnterScope scope("TestOperation", "test");
        spanId = scope.getSpan()->spanId();
    }

    {
        tempo_tracing::CurrentScope scope("test");
        auto span = scope.getSpan();
    }

    auto *currentContext = tempo_tracing::TraceContext::currentContext();
    ASSERT_TRUE (currentContext->isActive());
    ASSERT_EQ (1, currentContext->numSpans());
    auto top = currentContext->peekSpan();
    ASSERT_EQ (top->spanId(), spanId);
    ASSERT_EQ ("TestOperation", top->getOperationName());
    ASSERT_FALSE (top->isActive());
}

TEST_F(CurrentScope, ScopeConstructionFailsIfNoCurrentContext)
{
    tempo_tracing::CurrentScope scope;
    ASSERT_FALSE (scope.isValid());
    auto span = scope.getSpan();
    ASSERT_TRUE (span == nullptr);
}

TEST_F(CurrentScope, ScopeConstructionFailsIfNoSpecifiedContext)
{
    tempo_tracing::CurrentScope scope("test");
    ASSERT_FALSE (scope.isValid());
    auto span = scope.getSpan();
    ASSERT_TRUE (span == nullptr);
}
