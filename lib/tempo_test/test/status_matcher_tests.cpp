#include <gtest/gtest.h>

#include <tempo_test/status_matchers.h>

using namespace tempo_test;

TEST(StatusMatcher, MatchesOkStatus)
{
    tempo_utils::Status status{};

    ASSERT_THAT (status, MatchesStatusCode(tempo_utils::StatusCode::kOk));
}

TEST(StatusMatcher, MatchesStatusCode)
{
    auto status = tempo_utils::GenericStatus::forCondition(tempo_utils::GenericCondition::kInternalViolation);

    ASSERT_THAT (status, MatchesStatusCode(tempo_utils::StatusCode::kInternal));
}

TEST(StatusMatcher, MatchesErrorCategory)
{
    auto status = tempo_utils::GenericStatus::forCondition(tempo_utils::GenericCondition::kInternalViolation);

    ASSERT_THAT (status, MatchesErrorCategory(tempo_utils::kTempoUtilsGenericStatusNs));
    ASSERT_THAT (status, MatchesErrorCategory(std::string_view(tempo_utils::kTempoUtilsGenericStatusNs)));
}

TEST(StatusMatcher, IsCondition)
{
    auto status = tempo_utils::GenericStatus::forCondition(tempo_utils::GenericCondition::kInternalViolation);

    ASSERT_THAT (status, IsCondition(tempo_utils::GenericCondition::kInternalViolation));
}
