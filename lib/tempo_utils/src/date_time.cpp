
#include <absl/time/time.h>
#include <absl/time/clock.h>

#include <tempo_utils/date_time.h>

int64_t tempo_utils::seconds_since_epoch()
{
    return ToUnixSeconds(absl::Now());
}

int64_t tempo_utils::millis_since_epoch()
{
    return ToUnixMillis(absl::Now());
}
