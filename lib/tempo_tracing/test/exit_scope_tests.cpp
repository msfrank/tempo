#include <gtest/gtest.h>

#include <tempo_tracing/enter_scope.h>
#include <tempo_tracing/exit_scope.h>
#include <tempo_tracing/internal/thread_context.h>
#include <tempo_tracing/trace_recorder.h>

#include "base_tracing_fixture.h"

class ExitScope : public BaseTracingFixture {
protected:
    void TearDown() override {
        tempo_tracing::internal::reset_thread_context();
    }
};

TEST_F(ExitScope, PopSpanOnCurrentContextAfterScopeDestructs)
{
    tempo_tracing::TraceContext::makeContextAndSwitch("test");
    ASSERT_EQ ("test", tempo_tracing::TraceContext::currentContext()->getName());

    {
        tempo_tracing::EnterScope scope("TestOperation");
    }

    {
        tempo_tracing::ExitScope scope;
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

TEST_F(ExitScope, PopSpanOnSpecifiedContextAfterScopeDestructs)
{
    tempo_tracing::TraceContext::makeContext("test");
    ASSERT_TRUE (tempo_tracing::TraceContext::currentContext() == nullptr);

    {
        tempo_tracing::EnterScope scope("TestOperation", "test");
    }

    {
        tempo_tracing::ExitScope scope("test");
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

TEST_F(ExitScope, ScopeConstructionFailsIfNoCurrentContext)
{
    tempo_tracing::ExitScope scope;
    ASSERT_FALSE (scope.isValid());
    auto span = scope.getSpan();
    ASSERT_TRUE (span == nullptr);
}

TEST_F(ExitScope, ScopeConstructionFailsIfNoSpecifiedContext)
{
    tempo_tracing::ExitScope scope("test");
    ASSERT_FALSE (scope.isValid());
    auto span = scope.getSpan();
    ASSERT_TRUE (span == nullptr);
}
