#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_tracing/internal/spanset_reader.h>
#include <tempo_tracing/span_walker.h>
#include <tempo_tracing/trace_recorder.h>
#include <tempo_tracing/tracing_schema.h>

#include "base_tracing_fixture.h"
#include "tempo_test/result_matchers.h"
#include "tempo_test/status_matchers.h"

class SpanFailurePropagation : public BaseTracingFixture {};

TEST_F(SpanFailurePropagation, SpanFailureDoesNotPropagateToParentByDefault)
{
    auto parentSpan = recorder->makeSpan();
    auto childSpan = parentSpan->makeSpan(tempo_tracing::FailurePropagation::PropagatesToParent);

    childSpan->close(
        tempo_utils::GenericCondition::kInternalViolation,
        tempo_tracing::LogSeverity::kError);

    recorder->close();
    auto toSpansetResult = recorder->toSpanset();
    ASSERT_THAT (toSpansetResult, tempo_test::IsResult());
    auto spanset = toSpansetResult.getResult();

    auto reader = spanset.getReader();
    ASSERT_EQ (2, reader->numSpans());

    auto parentWalker = spanset.getRoots().getRoot(0);
    ASSERT_TRUE (parentWalker.isValid());
    ASSERT_EQ (1, parentWalker.numChildren());

    auto childWalker = parentWalker.getChild(0);
    ASSERT_TRUE (childWalker.isValid());

    // child span is failed
    ASSERT_TRUE (childWalker.isFailed());

    // parent span is not failed
    ASSERT_FALSE (parentWalker.isFailed());
}

TEST_F(SpanFailurePropagation, SpanFailurePropagatesWhenChildFails)
{
    auto parentSpan = recorder->makeSpan(
        tempo_tracing::FailurePropagation::NoPropagation, tempo_tracing::FailureCollection::AnyChildFailed);
    auto childSpan = parentSpan->makeSpan(tempo_tracing::FailurePropagation::PropagatesToParent);

    childSpan->close(
        tempo_utils::GenericCondition::kInternalViolation,
        tempo_tracing::LogSeverity::kError);

    recorder->close();
    auto toSpansetResult = recorder->toSpanset();
    ASSERT_THAT (toSpansetResult, tempo_test::IsResult());
    auto spanset = toSpansetResult.getResult();

    auto reader = spanset.getReader();
    ASSERT_EQ (2, reader->numSpans());

    auto parentWalker = spanset.getRoots().getRoot(0);
    ASSERT_TRUE (parentWalker.isValid());
    ASSERT_EQ (1, parentWalker.numChildren());

    auto childWalker = parentWalker.getChild(0);
    ASSERT_TRUE (childWalker.isValid());

    // child span is failed
    ASSERT_TRUE (childWalker.isFailed());

    // parent span is failed
    ASSERT_TRUE (parentWalker.isFailed());
}

TEST_F(SpanFailurePropagation, SpanFailurePropagatesWhenAnyChildFails)
{
    auto parentSpan = recorder->makeSpan(
        tempo_tracing::FailurePropagation::NoPropagation, tempo_tracing::FailureCollection::AnyChildFailed);
    auto childSpan1 = parentSpan->makeSpan(tempo_tracing::FailurePropagation::PropagatesToParent);
    auto childSpan2 = parentSpan->makeSpan(tempo_tracing::FailurePropagation::PropagatesToParent);
    auto childSpan3 = parentSpan->makeSpan(tempo_tracing::FailurePropagation::PropagatesToParent);

    childSpan1->close();
    childSpan2->close();
    childSpan3->close(
        tempo_utils::GenericCondition::kInternalViolation,
        tempo_tracing::LogSeverity::kError);

    recorder->close();
    auto toSpansetResult = recorder->toSpanset();
    ASSERT_THAT (toSpansetResult, tempo_test::IsResult());
    auto spanset = toSpansetResult.getResult();

    auto reader = spanset.getReader();
    ASSERT_EQ (4, reader->numSpans());

    auto parentWalker = spanset.getRoots().getRoot(0);
    ASSERT_TRUE (parentWalker.isValid());
    ASSERT_EQ (3, parentWalker.numChildren());

    auto childWalker1 = parentWalker.getChild(0);
    ASSERT_TRUE (childWalker1.isValid());
    auto childWalker2 = parentWalker.getChild(1);
    ASSERT_TRUE (childWalker2.isValid());
    auto childWalker3 = parentWalker.getChild(2);
    ASSERT_TRUE (childWalker3.isValid());

    // only child3 is failed
    ASSERT_FALSE (childWalker1.isFailed());
    ASSERT_FALSE (childWalker2.isFailed());
    ASSERT_TRUE (childWalker3.isFailed());

    // parent span is failed
    ASSERT_TRUE (parentWalker.isFailed());
}

TEST_F(SpanFailurePropagation, SpanFailurePropagatesWhenAllChildrenFail)
{
    auto parentSpan = recorder->makeSpan(
        tempo_tracing::FailurePropagation::NoPropagation, tempo_tracing::FailureCollection::AllChildrenFailed);
    auto childSpan1 = parentSpan->makeSpan(tempo_tracing::FailurePropagation::PropagatesToParent);
    auto childSpan2 = parentSpan->makeSpan(tempo_tracing::FailurePropagation::PropagatesToParent);
    auto childSpan3 = parentSpan->makeSpan(tempo_tracing::FailurePropagation::PropagatesToParent);

    childSpan1->close(
        tempo_utils::GenericCondition::kInternalViolation,
        tempo_tracing::LogSeverity::kError);
    childSpan2->close(
        tempo_utils::GenericCondition::kInternalViolation,
        tempo_tracing::LogSeverity::kError);
    childSpan3->close(
        tempo_utils::GenericCondition::kInternalViolation,
        tempo_tracing::LogSeverity::kError);

    recorder->close();
    auto toSpansetResult = recorder->toSpanset();
    ASSERT_THAT (toSpansetResult, tempo_test::IsResult());
    auto spanset = toSpansetResult.getResult();

    auto reader = spanset.getReader();
    ASSERT_EQ (4, reader->numSpans());

    auto parentWalker = spanset.getRoots().getRoot(0);
    ASSERT_TRUE (parentWalker.isValid());
    ASSERT_EQ (3, parentWalker.numChildren());

    auto childWalker1 = parentWalker.getChild(0);
    ASSERT_TRUE (childWalker1.isValid());
    auto childWalker2 = parentWalker.getChild(1);
    ASSERT_TRUE (childWalker2.isValid());
    auto childWalker3 = parentWalker.getChild(2);
    ASSERT_TRUE (childWalker3.isValid());

    // all children are failed
    ASSERT_TRUE (childWalker1.isFailed());
    ASSERT_TRUE (childWalker2.isFailed());
    ASSERT_TRUE (childWalker3.isFailed());

    // parent span is failed
    ASSERT_TRUE (parentWalker.isFailed());
}

TEST_F(SpanFailurePropagation, SpanFailureDoesNotPropagateWhenAtLeastOneChildSucceeds)
{
    auto parentSpan = recorder->makeSpan(
        tempo_tracing::FailurePropagation::NoPropagation, tempo_tracing::FailureCollection::AllChildrenFailed);
    auto childSpan1 = parentSpan->makeSpan(tempo_tracing::FailurePropagation::PropagatesToParent);
    auto childSpan2 = parentSpan->makeSpan(tempo_tracing::FailurePropagation::PropagatesToParent);
    auto childSpan3 = parentSpan->makeSpan(tempo_tracing::FailurePropagation::PropagatesToParent);

    childSpan1->close(
        tempo_utils::GenericCondition::kInternalViolation,
        tempo_tracing::LogSeverity::kError);
    childSpan2->close();
    childSpan3->close(
        tempo_utils::GenericCondition::kInternalViolation,
        tempo_tracing::LogSeverity::kError);

    recorder->close();
    auto toSpansetResult = recorder->toSpanset();
    ASSERT_THAT (toSpansetResult, tempo_test::IsResult());
    auto spanset = toSpansetResult.getResult();

    auto reader = spanset.getReader();
    ASSERT_EQ (4, reader->numSpans());

    auto parentWalker = spanset.getRoots().getRoot(0);
    ASSERT_TRUE (parentWalker.isValid());
    ASSERT_EQ (3, parentWalker.numChildren());

    auto childWalker1 = parentWalker.getChild(0);
    ASSERT_TRUE (childWalker1.isValid());
    auto childWalker2 = parentWalker.getChild(1);
    ASSERT_TRUE (childWalker2.isValid());
    auto childWalker3 = parentWalker.getChild(2);
    ASSERT_TRUE (childWalker3.isValid());

    // child 1 and 3 failed, child 2 succeeded
    ASSERT_TRUE (childWalker1.isFailed());
    ASSERT_FALSE (childWalker2.isFailed());
    ASSERT_TRUE (childWalker3.isFailed());

    // parent span is not failed
    ASSERT_FALSE (parentWalker.isFailed());
}

TEST_F(SpanFailurePropagation, SpanBlocksPropagation)
{
    // span1 succeeds, collects failure
    auto span1 = recorder->makeSpan(tempo_tracing::FailurePropagation::NoPropagation,
        tempo_tracing::FailureCollection::AnyChildFailed);

    // span2 succeeds, collects failure, propagates to span1
    auto span2 = span1->makeSpan(tempo_tracing::FailurePropagation::PropagatesToParent,
        tempo_tracing::FailureCollection::AnyChildFailed);

    // span3 fails, propagates to span2.
    auto span3 = span2->makeSpan(tempo_tracing::FailurePropagation::PropagatesToParent,
        tempo_tracing::FailureCollection::AnyChildFailed);

    // span4 succeeds
    auto span4 = span3->makeSpan(tempo_tracing::FailurePropagation::PropagatesToParent,
        tempo_tracing::FailureCollection::AnyChildFailed);

    // span5 succeeds, collects failure, does not propagate to span4.
    auto span5 = span4->makeSpan(tempo_tracing::FailurePropagation::NoPropagation,
        tempo_tracing::FailureCollection::AnyChildFailed);

    // span6 fails, propagates to span5.
    auto span6 = span5->makeSpan(tempo_tracing::FailurePropagation::PropagatesToParent);

    span1->close();
    span2->close();
    span3->close(
        tempo_utils::GenericCondition::kInternalViolation,
        tempo_tracing::LogSeverity::kError);
    span4->close();
    span5->close();
    span6->close(
        tempo_utils::GenericCondition::kInternalViolation,
        tempo_tracing::LogSeverity::kError);

    recorder->close();
    auto toSpansetResult = recorder->toSpanset();
    ASSERT_THAT (toSpansetResult, tempo_test::IsResult());
    auto spanset = toSpansetResult.getResult();

    auto reader = spanset.getReader();
    ASSERT_EQ (6, reader->numSpans());

    auto walker1 = spanset.getRoots().getRoot(0);
    ASSERT_TRUE (walker1.isValid());
    auto walker2 = walker1.getChild(0);
    ASSERT_TRUE (walker2.isValid());
    auto walker3 = walker2.getChild(0);
    ASSERT_TRUE (walker3.isValid());
    auto walker4 = walker3.getChild(0);
    ASSERT_TRUE (walker4.isValid());
    auto walker5 = walker4.getChild(0);
    ASSERT_TRUE (walker5.isValid());
    auto walker6 = walker5.getChild(0);
    ASSERT_TRUE (walker6.isValid());

    ASSERT_TRUE (walker1.isFailed());
    ASSERT_TRUE (walker2.isFailed());
    ASSERT_TRUE (walker3.isFailed());
    ASSERT_FALSE (walker4.isFailed());
    ASSERT_TRUE (walker5.isFailed());
    ASSERT_TRUE (walker6.isFailed());
}
