
#include <tempo_test/result_matchers.h>

::testing::Matcher<tempo_utils::MaybeStatus<tempo_utils::Status>>
tempo_test::IsStatus()
{
    return ::testing::Not(MaybeStatusMatcher<tempo_utils::Status>());
}

::testing::Matcher<tempo_utils::MaybeStatus<tempo_utils::Status>>
tempo_test::IsResult()
{
    return MaybeStatusMatcher<tempo_utils::Status>();
}
