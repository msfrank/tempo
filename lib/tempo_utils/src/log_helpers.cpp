
#include <tempo_utils/log_helpers.h>
#include <tempo_utils/log_stream.h>

tempo_utils::LogMessage&& tempo_utils::operator<<(tempo_utils::LogMessage &&message, tempo_utils::Indent indent)
{
    char spacer[indent.indent + 1];
    for (tu_uint32 i = 0; i < indent.indent; i++) {
        spacer[i] = ' ';
    }
    spacer[indent.indent] = '\0';

    std::forward<tempo_utils::LogMessage>(message) << spacer;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(tempo_utils::LogMessage &&message, tempo_utils::Fill fill)
{
    char spacer[fill.count + 1];
    for (tu_uint32 i = 0; i < fill.count; i++) {
        spacer[i] = fill.ch;
    }
    spacer[fill.count] = '\0';

    std::forward<tempo_utils::LogMessage>(message) << spacer;
    return std::move(message);
}
