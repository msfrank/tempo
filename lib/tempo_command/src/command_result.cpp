#include <absl/strings/str_join.h>
#include <absl/strings/substitute.h>

#include <tempo_command/command_result.h>

tempo_command::CommandStatus::CommandStatus(
    tempo_utils::StatusCode statusCode,
    std::shared_ptr<const tempo_utils::Detail> detail)
    : tempo_utils::TypedStatus<CommandCondition>(statusCode, detail)
{
}

tempo_command::CommandStatus
tempo_command::CommandStatus::ok()
{
    return CommandStatus();
}

bool
tempo_command::CommandStatus::convert(CommandStatus &dstStatus, const tempo_utils::Status &srcStatus)
{
    std::string_view srcNs = srcStatus.getErrorCategory();
    std::string_view dstNs = kTempoCommandStatusNs;
    if (srcNs != dstNs)
        return false;
    dstStatus = CommandStatus(srcStatus.getStatusCode(), srcStatus.getDetail());
    return true;
}
