
#include <absl/strings/substitute.h>

#include <tempo_utils/log_stream.h>

tempo_utils::LogMessage&& tempo_utils::operator<<(tempo_utils::LogMessage &&message, const char *str)
{
    message.m_buffer << str;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(tempo_utils::LogMessage &&message, const std::string &str)
{
    message.m_buffer << str;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(tempo_utils::LogMessage &&message, const void *p)
{
    message.m_buffer << absl::Substitute("$0", p);
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(tempo_utils::LogMessage &&message, tu_int16 i16)
{
    message.m_buffer << i16;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(tempo_utils::LogMessage &&message, tu_uint16 u16)
{
    message.m_buffer << u16;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(tempo_utils::LogMessage &&message, tu_int32 i32)
{
    message.m_buffer << i32;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(tempo_utils::LogMessage &&message, tu_uint32 u32)
{
    message.m_buffer << u32;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(tempo_utils::LogMessage &&message, tu_int64 i64)
{
    message.m_buffer << i64;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(tempo_utils::LogMessage &&message, tu_uint64 u64)
{
    message.m_buffer << u64;
    return std::move(message);
}

/*
tempo_utils::LogMessage&& tempo_utils::operator<<(tempo_utils::LogMessage &&message, size_t st)
{
    message.m_buffer << st;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(tempo_utils::LogMessage &&message, ssize_t sst)
{
    message.m_buffer << sst;
    return std::move(message);
}
*/

tempo_utils::LogMessage&& tempo_utils::operator<<(tempo_utils::LogMessage &&message, float flt)
{
    message.m_buffer << flt;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(tempo_utils::LogMessage &&message, double dbl)
{
    message.m_buffer << dbl;
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(tempo_utils::LogMessage &&message, bool b)
{
    message.m_buffer << (b? "true" : "false");
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(tempo_utils::LogMessage &&message, const absl::Dec &dec)
{
    message.m_buffer << absl::StrCat(dec);
    return std::move(message);
}

tempo_utils::LogMessage&& tempo_utils::operator<<(tempo_utils::LogMessage &&message, const absl::Hex &hex)
{
    message.m_buffer << absl::StrCat(hex);
    return std::move(message);
}
