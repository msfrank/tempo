
#include <tempo_utils/file_result.h>

tempo_utils::FileStatus::FileStatus(
    tempo_utils::StatusCode statusCode,
    std::shared_ptr<const tempo_utils::Detail> detail)
    : tempo_utils::TypedStatus<FileCondition>(statusCode, detail)
{
}

tempo_utils::FileStatus
tempo_utils::FileStatus::ok()
{
    return FileStatus();
}

bool
tempo_utils::FileStatus::convert(FileStatus &dstStatus, const tempo_utils::Status &srcStatus)
{
    std::string_view srcNs = srcStatus.getErrorCategory();
    std::string_view dstNs = kTempoUtilsFileStatusNs;
    if (srcNs != dstNs)
        return false;
    dstStatus = FileStatus(srcStatus.getStatusCode(), srcStatus.getDetail());
    return true;
}
