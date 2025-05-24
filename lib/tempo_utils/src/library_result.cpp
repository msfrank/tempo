
#include <tempo_utils/library_result.h>

tempo_utils::LibraryStatus::LibraryStatus(
    tempo_utils::StatusCode statusCode,
    std::shared_ptr<const tempo_utils::Detail> detail)
    : tempo_utils::TypedStatus<LibraryCondition>(statusCode, detail)
{
}

tempo_utils::LibraryStatus
tempo_utils::LibraryStatus::ok()
{
    return LibraryStatus();
}

bool
tempo_utils::LibraryStatus::convert(LibraryStatus &dstStatus, const tempo_utils::Status &srcStatus)
{
    std::string_view srcNs = srcStatus.getErrorCategory();
    std::string_view dstNs = kTempoUtilsLibraryStatusNs;
    if (srcNs != dstNs)
        return false;
    dstStatus = LibraryStatus(srcStatus.getStatusCode(), srcStatus.getDetail());
    return true;
}
