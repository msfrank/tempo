
#include <absl/strings/substitute.h>

#include <tempo_utils/log_stream.h>

/**
 * Stream a char* to the LogMessage.
 *
 * @param message The receiver of the string
 * @param str The char* string to send to the receiver
 * @return The receiver
 */
tempo_utils::LogMessage&& tempo_utils::operator<<(LogMessage &&message, const char *str)
{
    message.m_buffer << str;
    return std::move(message);
}

/**
 * Stream a std::string to the LogMessage.
 *
 * @param message The receiver of the string
 * @param str The std::string object to send to the receiver
 * @return The receiver
 */
tempo_utils::LogMessage&& tempo_utils::operator<<(LogMessage &&message, const std::string &str)
{
    message.m_buffer << str;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(LogMessage &&message, std::string_view view)
{
    message.m_buffer << view;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(LogMessage &&message, const void *p)
{
    message.m_buffer << absl::Substitute("$0", p);
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(LogMessage &&message, tu_int16 i16)
{
    message.m_buffer << i16;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(LogMessage &&message, tu_uint16 u16)
{
    message.m_buffer << u16;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(LogMessage &&message, tu_int32 i32)
{
    message.m_buffer << i32;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(LogMessage &&message, tu_uint32 u32)
{
    message.m_buffer << u32;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(LogMessage &&message, tu_int64 i64)
{
    message.m_buffer << i64;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(LogMessage &&message, tu_uint64 u64)
{
    message.m_buffer << u64;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(LogMessage &&message, float flt)
{
    message.m_buffer << flt;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(LogMessage &&message, double dbl)
{
    message.m_buffer << dbl;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(LogMessage &&message, bool b)
{
    message.m_buffer << (b? "true" : "false");
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(LogMessage &&message, const absl::Dec &dec)
{
    message.m_buffer << absl::StrCat(dec);
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(LogMessage &&message, const absl::Hex &hex)
{
    message.m_buffer << absl::StrCat(hex);
    return std::move(message);
}
