#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_tracing/span_walker.h>
#include <tempo_tracing/trace_recorder.h>
#include <tempo_test/result_matchers.h>
#include <tempo_test/status_matchers.h>
#include <tempo_utils/date_time.h>

#include "base_tracing_fixture.h"

class TraceSpanTiming : public BaseTracingFixture {};

TEST_F(TraceSpanTiming, InitialTimesAreInvalid)
{
    recorder = tempo_tracing::TraceRecorder::create();
    auto span = recorder->makeSpan();

    auto initialStartTime = span->getStartTime();
    ASSERT_EQ (-1, ToUnixSeconds(initialStartTime));
    auto initialEndTime = span->getEndTime();
    ASSERT_EQ (-1, ToUnixSeconds(initialEndTime));
    auto initialActiveDuration = span->getActiveDuration();
    ASSERT_EQ (0, ToInt64Nanoseconds(initialActiveDuration));
}

TEST_F(TraceSpanTiming, FirstActivationSetsStartTime)
{
    recorder = tempo_tracing::TraceRecorder::create();
    auto span = recorder->makeSpan();

    auto initialStartTime = span->getStartTime();
    ASSERT_EQ (-1, ToUnixSeconds(initialStartTime));

    auto beforeActivate = tempo_utils::millis_since_epoch();

    span->activate();
    auto startTime = ToUnixMillis(span->getStartTime());
    ASSERT_LE (beforeActivate, startTime);
}

TEST_F(TraceSpanTiming, DeactivationSetsEndTimeAndActiveDuration)
{
    recorder = tempo_tracing::TraceRecorder::create();
    auto span = recorder->makeSpan();

    auto initialStartTime = span->getStartTime();
    ASSERT_EQ (-1, ToUnixSeconds(initialStartTime));

    auto beforeActivate = tempo_utils::millis_since_epoch();

    span->activate();
    tu_int64 sleepDuration = 10;
    absl::SleepFor(absl::Milliseconds(sleepDuration));
    span->deactivate();

    auto afterDeactivate = tempo_utils::millis_since_epoch();

    auto endTime = ToUnixMillis(span->getEndTime());
    ASSERT_GE (afterDeactivate, endTime);

    auto activeDuration = ToInt64Milliseconds(span->getActiveDuration());
    ASSERT_GE (afterDeactivate - beforeActivate, activeDuration);
    ASSERT_GE (5.0, std::abs(activeDuration - sleepDuration));
}

TEST_F(TraceSpanTiming, CloseSetsEndTimeAndActiveDuration)
{
    recorder = tempo_tracing::TraceRecorder::create();
    auto span = recorder->makeSpan();

    auto initialStartTime = span->getStartTime();
    ASSERT_EQ (-1, ToUnixSeconds(initialStartTime));

    auto beforeActivate = tempo_utils::millis_since_epoch();

    span->activate();
    tu_int64 sleepDuration = 10;
    absl::SleepFor(absl::Milliseconds(sleepDuration));
    span->close();

    auto afterClose = tempo_utils::millis_since_epoch();

    auto endTime = ToUnixMillis(span->getEndTime());
    ASSERT_GE (afterClose, endTime);

    auto activeDuration = ToInt64Milliseconds(span->getActiveDuration());
    ASSERT_GE (afterClose - beforeActivate, activeDuration);
    ASSERT_GE (5.0, std::abs(activeDuration - sleepDuration));
}
