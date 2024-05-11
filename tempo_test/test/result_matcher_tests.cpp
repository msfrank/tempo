#include <gtest/gtest.h>

#include <tempo_test/result_matchers.h>

using namespace tempo_test;

TEST(ResultMatcher, ContainsResult)
{
    auto result = tempo_utils::Result<int>(42);

    ASSERT_THAT (result, ContainsResult(
        ::testing::Matcher<int>(::testing::Eq(42))));
}

TEST(ResultMatcher, ContainsStatus)
{
    auto result = tempo_utils::Result<int>(
        tempo_utils::GenericStatus::forCondition(tempo_utils::GenericCondition::kInternalViolation));

    ASSERT_THAT (result, ContainsStatus(tempo_utils::GenericCondition::kInternalViolation));
}
