
#include <tempo_test/result_matchers.h>

::testing::Matcher<tempo_utils::MaybeStatus<tempo_utils::Status>>
tempo_test::IsStatus()
{
    return MaybeStatusMatcher<tempo_utils::Status>();
}
