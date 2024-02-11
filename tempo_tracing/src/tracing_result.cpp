#include <absl/strings/substitute.h>

#include <tempo_tracing/tracing_result.h>

tempo_tracing::TracingStatus::TracingStatus(
    tempo_utils::StatusCode statusCode,
    std::shared_ptr<const tempo_utils::Detail> detail)
    : tempo_utils::TypedStatus<TracingCondition>(statusCode, detail)
{
}

tempo_tracing::TracingStatus
tempo_tracing::TracingStatus::ok()
{
    return TracingStatus();
}

bool
tempo_tracing::TracingStatus::convert(TracingStatus &dstStatus, const tempo_utils::Status &srcStatus)
{
    std::string_view srcNs = srcStatus.getErrorCategory();
    std::string_view dstNs = kTempoTracingStatusNs.getNs();
    if (srcNs != dstNs)
        return false;
    dstStatus = TracingStatus(srcStatus.getStatusCode(), srcStatus.getDetail());
    return true;
}
