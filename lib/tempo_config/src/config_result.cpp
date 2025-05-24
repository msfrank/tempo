
#include <tempo_config/config_result.h>

tempo_config::ConfigStatus::ConfigStatus(
    tempo_utils::StatusCode statusCode,
    std::shared_ptr<const tempo_utils::Detail> detail)
    : tempo_utils::TypedStatus<ConfigCondition>(statusCode, detail)
{
}

tempo_config::ConfigStatus
tempo_config::ConfigStatus::ok()
{
    return ConfigStatus();
}

bool
tempo_config::ConfigStatus::convert(ConfigStatus &dstStatus, const tempo_utils::Status &srcStatus)
{
    std::string_view srcNs = srcStatus.getErrorCategory();
    std::string_view dstNs = kTempoUtilsConfigStatusNs;
    if (srcNs != dstNs)
        return false;
    dstStatus = ConfigStatus(srcStatus.getStatusCode(), srcStatus.getDetail());
    return true;
}
