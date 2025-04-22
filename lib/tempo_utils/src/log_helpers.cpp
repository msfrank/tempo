
#include <tempo_utils/log_helpers.h>
#include <tempo_utils/log_stream.h>

tempo_utils::LogMessage&& tempo_utils::operator<<(LogMessage &&message, Indent indent)
{
    std::string spacer(indent.indent, ' ');
    std::forward<tempo_utils::LogMessage>(message) << spacer;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(LogMessage &&message, Fill fill)
{
    std::string filler(fill.count, fill.ch);
    std::forward<tempo_utils::LogMessage>(message) << filler;
    return std::move(message);
}
