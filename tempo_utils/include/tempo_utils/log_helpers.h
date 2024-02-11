#ifndef TEMPO_UTILS_LOG_HELPERS_H
#define TEMPO_UTILS_LOG_HELPERS_H

#include "log_message.h"

namespace tempo_utils {

    struct Indent {
        unsigned int indent;
        Indent(unsigned int indent) : indent(indent) {};
    };

    LogMessage&& operator<<(LogMessage &&message, Indent indent);

    struct Fill {
        char ch;
        unsigned int count;
        Fill(char ch, unsigned int count) : ch(ch), count(count) {};
    };

    LogMessage&& operator<<(LogMessage &&message, Fill fill);
};

#endif // TEMPO_UTILS_LOG_HELPERS_H
