
#include <tempo_utils/attr_result.h>

tempo_utils::AttrStatus::AttrStatus(
    tempo_utils::StatusCode statusCode,
    std::shared_ptr<const tempo_utils::Detail> detail)
    : tempo_utils::TypedStatus<AttrCondition>(statusCode, detail)
{
}

tempo_utils::AttrStatus
tempo_utils::AttrStatus::ok()
{
    return AttrStatus();
}

bool
tempo_utils::AttrStatus::convert(AttrStatus &dstStatus, const tempo_utils::Status &srcStatus)
{
    std::string_view srcNs = srcStatus.getErrorCategory();
    std::string_view dstNs = kTempoUtilsAttrStatusNs.getNs();
    if (srcNs != dstNs)
        return false;
    dstStatus = AttrStatus(srcStatus.getStatusCode(), srcStatus.getDetail());
    return true;
}