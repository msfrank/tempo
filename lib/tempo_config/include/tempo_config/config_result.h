#ifndef TEMPO_CONFIG_CONFIG_RESULT_H
#define TEMPO_CONFIG_CONFIG_RESULT_H

#include <string>

#include <fmt/core.h>
#include <fmt/format.h>

#include <tempo_utils/result.h>
#include <tempo_utils/status.h>

namespace tempo_config {

    constexpr const char *kTempoUtilsConfigStatusNs = "dev.zuri.ns:tempo-config-status-1";

    enum class ConfigCondition {
        kParseError,
        kMissingValue,
        kWrongType,
        kConfigInvariant,
    };

    class ConfigStatus : public tempo_utils::TypedStatus<ConfigCondition> {
    public:
        using TypedStatus::TypedStatus;
        static bool convert(ConfigStatus &dstStatus, const tempo_utils::Status &srcStatus);

    private:
        ConfigStatus(tempo_utils::StatusCode statusCode, std::shared_ptr<const tempo_utils::Detail> detail);

    public:
        /**
         *
         * @param condition
         * @param message
         * @return
         */
        static ConfigStatus forCondition(
            ConfigCondition condition,
            std::string_view message)
        {
            return ConfigStatus(condition, message);
        }
        /**
         *
         * @tparam Args
         * @param condition
         * @param messageFmt
         * @param messageArgs
         * @return
         */
        template<typename... Args>
        static ConfigStatus forCondition(
            ConfigCondition condition,
            fmt::string_view messageFmt = {},
            Args... messageArgs) {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            return ConfigStatus(condition, message);
        }

        /**
         *
         * @tparam Args
         * @param condition
         * @param messageFmt
         * @param messageArgs
         * @return
         */
        template<typename... Args>
        static ConfigStatus forCondition(
            ConfigCondition condition,
            tempo_utils::TraceId traceId,
            tempo_utils::SpanId spanId,
            fmt::string_view messageFmt = {},
            Args... messageArgs) {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            return ConfigStatus(condition, message, traceId, spanId);
        }
    };

    template<typename T>
    class ConfigResult : public tempo_utils::TypedResult<T, ConfigStatus> {
    public:
        ConfigResult() : tempo_utils::TypedResult<T, ConfigStatus>() {};
        ConfigResult(const T &result) : tempo_utils::TypedResult<T, ConfigStatus>(result) {};
        ConfigResult(const ConfigStatus &status) : tempo_utils::TypedResult<T, ConfigStatus>(status) {};
    };
}

namespace tempo_utils {

    template<>
    struct StatusTraits<tempo_config::ConfigStatus> {
        using ConditionType = tempo_config::ConfigCondition;
        static bool convert(tempo_config::ConfigStatus &dstStatus, const tempo_utils::Status &srcStatus)
        {
            return tempo_config::ConfigStatus::convert(dstStatus, srcStatus);
        }
    };

    template<>
    struct ConditionTraits<tempo_config::ConfigCondition> {
        using StatusType = tempo_config::ConfigStatus;
        static constexpr const char *condition_namespace() { return tempo_config::kTempoUtilsConfigStatusNs; }
        static constexpr StatusCode make_status_code(tempo_config::ConfigCondition condition)
        {
            switch (condition) {
                case tempo_config::ConfigCondition::kParseError:
                    return tempo_utils::StatusCode::kInternal;
                case tempo_config::ConfigCondition::kMissingValue:
                    return tempo_utils::StatusCode::kNotFound;
                case tempo_config::ConfigCondition::kWrongType:
                    return tempo_utils::StatusCode::kInvalidArgument;
                case tempo_config::ConfigCondition::kConfigInvariant:
                    return tempo_utils::StatusCode::kInternal;
                default:
                    return tempo_utils::StatusCode::kUnknown;
            }
        };
        static constexpr const char *make_error_message(tempo_config::ConfigCondition condition)
        {
            switch (condition) {
                case tempo_config::ConfigCondition::kParseError:
                    return "Parse error";
                case tempo_config::ConfigCondition::kMissingValue:
                    return "Missing value";
                case tempo_config::ConfigCondition::kWrongType:
                    return "Wrong type";
                case tempo_config::ConfigCondition::kConfigInvariant:
                    return "Config invariant";
                default:
                    return "INVALID";
            }
        }
    };
}

#endif // TEMPO_CONFIG_CONFIG_RESULT_H