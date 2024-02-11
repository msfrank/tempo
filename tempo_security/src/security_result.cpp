
#include <tempo_security/security_result.h>

tempo_security::SecurityStatus::SecurityStatus(
    tempo_utils::StatusCode statusCode,
    std::shared_ptr<const tempo_utils::Detail> detail)
    : tempo_utils::TypedStatus<SecurityCondition>(statusCode, detail)
{
}

tempo_security::SecurityStatus
tempo_security::SecurityStatus::ok()
{
    return SecurityStatus();
}

bool
tempo_security::SecurityStatus::convert(SecurityStatus &dstStatus, const tempo_utils::Status &srcStatus)
{
    std::string_view srcNs = srcStatus.getErrorCategory();
    std::string_view dstNs = kTempoSecurityStatusNs.getNs();
    if (srcNs != dstNs)
        return false;
    dstStatus = SecurityStatus(srcStatus.getStatusCode(), srcStatus.getDetail());
    return true;
}