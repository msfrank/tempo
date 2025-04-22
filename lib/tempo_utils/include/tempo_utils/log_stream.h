#ifndef TEMPO_UTILS_LOG_STREAM_H
#define TEMPO_UTILS_LOG_STREAM_H

#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>
#include <absl/strings/str_cat.h>

#include "integer_types.h"
#include "log_console.h"
#include "log_message.h"

namespace tempo_utils {

    /**
     * @brief Stream a char* to the LogMessage.
     * @param message The receiver of the string
     * @param str The char* string to send to the receiver
     * @return The receiver
     */
    LogMessage&& operator<<(LogMessage &&message, const char *str);

    /**
     * @brief Stream a std::string to the LogMessage.
     * @param message The receiver of the string
     * @param str The std::string object to send to the receiver
     * @return The receiver
     */
    LogMessage&& operator<<(LogMessage &&message, const std::string &str);

    LogMessage&& operator<<(LogMessage &&message, const void *p);

    LogMessage&& operator<<(LogMessage &&message, tu_int16 i16);

    LogMessage&& operator<<(LogMessage &&message, tu_uint16 u16);

    LogMessage&& operator<<(LogMessage &&message, tu_int32 i32);

    LogMessage&& operator<<(LogMessage &&message, tu_uint32 u32);

    LogMessage&& operator<<(LogMessage &&message, tu_int64 i64);

    LogMessage&& operator<<(LogMessage &&message, tu_uint64 u64);

    //LogMessage&& operator<<(LogMessage &&message, size_t st);

    //LogMessage&& operator<<(LogMessage &&message, ssize_t sst);

    LogMessage&& operator<<(LogMessage &&message, float flt);

    LogMessage&& operator<<(LogMessage &&message, double dbl);

    LogMessage&& operator<<(LogMessage &&message, bool b);

    LogMessage&& operator<<(LogMessage &&message, const absl::Dec &dec);

    LogMessage&& operator<<(LogMessage &&message, const absl::Hex &hex);

    template<class T>
    LogMessage&& operator<<(LogMessage &&message, const absl::flat_hash_set<T> &set)
    {
        std::forward<LogMessage>(message) << "{";
        auto iterator = set.cbegin();
        if (iterator != set.cend()) {
            std::forward<LogMessage>(message) << *iterator;
            for (++iterator; iterator != set.cend(); iterator++) {
                std::forward<LogMessage>(message) << ", ";
                std::forward<LogMessage>(message) << *iterator;
            }
        }
        std::forward<LogMessage>(message) << "}";
        return std::move(message);
    }

    template<class K, class V>
    LogMessage&& operator<<(LogMessage &&message, const absl::flat_hash_map<K,V> &map)
    {
        std::forward<LogMessage>(message) << "{";
        auto iterator = map.cbegin();
        if (iterator != map.cend()) {
            std::forward<LogMessage>(message) << iterator->first;
            std::forward<LogMessage>(message) << ": ";
            std::forward<LogMessage>(message) << iterator->second;
            for (++iterator; iterator != map.cend(); iterator++) {
                std::forward<LogMessage>(message) << ", ";
                std::forward<LogMessage>(message) << iterator->first;
                std::forward<LogMessage>(message) << ": ";
                std::forward<LogMessage>(message) << iterator->second;
            }
        }
        std::forward<LogMessage>(message) << "}";
        return std::move(message);
    }
}

#endif // TEMPO_UTILS_LOG_STREAM_H
