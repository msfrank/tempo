#ifndef TEMPO_UTILS_LOG_HELPERS_H
#define TEMPO_UTILS_LOG_HELPERS_H

#include "log_message.h"
#include "log_stream.h"

namespace tempo_utils {

    /**
     * Print the specified amount of whitespace characters.
     */
    struct Indent {
        unsigned int indent;
        explicit Indent(unsigned int indent) : indent(indent) {}
    };

    LogMessage&& operator<<(LogMessage &&message, Indent indent);

    /**
     * Print the specified amount `count` of the specified character `ch`.
     */
    struct Fill {
        char ch;
        unsigned int count;
        Fill(char ch, unsigned int count) : ch(ch), count(count) {}
    };

    LogMessage&& operator<<(LogMessage &&message, Fill fill);

    /**
     * Print the specified integral `number` in binary.
     *
     * @tparam IntegralType
     */
    template<class IntegralType, std::enable_if_t<std::is_integral<IntegralType>::value, bool> = true>
    struct Bin {
        IntegralType number;
        bool showprefix;
        std::string prefix;
        explicit Bin(IntegralType number, bool showprefix = true, std::string_view prefix = "0b")
            : number(number), showprefix(showprefix), prefix(prefix) {}
    };

    template<class IntegralType, std::enable_if_t<std::is_integral<IntegralType>::value, bool> = true>
    LogMessage&& operator<<(LogMessage &&message, Bin<IntegralType> bin)
    {
        if (bin.showprefix && !bin.prefix.empty()) {
            std::forward<LogMessage>(message) << bin.prefix;
        }
        std::bitset<sizeof(IntegralType) * 8> bits(bin.number);
        std::forward<LogMessage>(message) << bits.to_string();
        return std::move(message);
    }

    /**
     * Print the specified integral `number` in hexadecimal.
     *
     * @tparam IntegralType
     */
    template<class IntegralType, std::enable_if_t<std::is_integral<IntegralType>::value, bool> = true>
    struct Hex {
        IntegralType number;
        bool showprefix;
        std::string prefix;
        explicit Hex(IntegralType number, bool showprefix = true, std::string_view prefix = "0x")
            : number(number), showprefix(showprefix), prefix(prefix) {}
    };

    template<class IntegralType, std::enable_if_t<std::is_integral<IntegralType>::value, bool> = true>
    LogMessage&& operator<<(LogMessage &&message, Hex<IntegralType> hex)
    {
        if (hex.showprefix && !hex.prefix.empty()) {
            std::forward<LogMessage>(message) << hex.prefix;
        }
        std::forward<LogMessage>(message) << absl::StrCat(absl::Hex(hex.number));
        return std::move(message);
    }
}

#endif // TEMPO_UTILS_LOG_HELPERS_H
