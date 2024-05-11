#ifndef TEMPO_UTILS_ATTR_RESULT_H
#define TEMPO_UTILS_ATTR_RESULT_H

#include <string>

#include <fmt/core.h>
#include <fmt/format.h>

#include "status.h"

namespace tempo_utils {

    constexpr tempo_utils::SchemaNs kTempoUtilsAttrStatusNs("dev.zuri.ns:tempo-utils-attr-status-1");

    enum class AttrCondition {
        kParseError,
        kConversionError,
        kMissingValue,
        kWrongType,
    };

    class AttrStatus : public TypedStatus<AttrCondition> {
    public:
        using TypedStatus::TypedStatus;
        static AttrStatus ok();
        static bool convert(AttrStatus &dstStatus, const tempo_utils::Status &srcStatus);

    private:
        AttrStatus(tempo_utils::StatusCode statusCode, std::shared_ptr<const tempo_utils::Detail> detail);

    public:
        /**
         *
         * @tparam Args
         * @param condition
         * @param messageFmt
         * @param messageArgs
         * @return
         */
        template <typename... Args>
        static AttrStatus forCondition(
            AttrCondition condition,
            fmt::string_view messageFmt = {},
            Args... messageArgs)
        {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            return AttrStatus(condition, message);
        }
        /**
         *
         * @tparam Args
         * @param condition
         * @param messageFmt
         * @param messageArgs
         * @return
         */
        template <typename... Args>
        static AttrStatus forCondition(
            AttrCondition condition,
            tempo_utils::TraceId traceId,
            tempo_utils::SpanId spanId,
            fmt::string_view messageFmt = {},
            Args... messageArgs)
        {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            return AttrStatus(condition, message, traceId, spanId);
        }
    };

    template<>
    struct StatusTraits<AttrStatus> {
        using ConditionType = AttrCondition;
        static bool convert(AttrStatus &dstStatus, const tempo_utils::Status &srcStatus)
        {
            return AttrStatus::convert(dstStatus, srcStatus);
        }
    };

    template<>
    struct ConditionTraits<AttrCondition> {
        using StatusType = AttrStatus;
        static constexpr const char *condition_namespace() { return kTempoUtilsAttrStatusNs.getNs(); }
        static constexpr StatusCode make_status_code(AttrCondition condition)
        {
            switch (condition) {
                case AttrCondition::kParseError:
                    return tempo_utils::StatusCode::kInternal;
                case AttrCondition::kConversionError:
                    return tempo_utils::StatusCode::kInvalidArgument;
                case AttrCondition::kMissingValue:
                    return tempo_utils::StatusCode::kNotFound;
                case AttrCondition::kWrongType:
                    return tempo_utils::StatusCode::kInvalidArgument;
                default:
                    return tempo_utils::StatusCode::kUnknown;
            }
        };
        static constexpr const char *make_error_message(AttrCondition condition)
        {
            switch (condition) {
                case AttrCondition::kParseError:
                    return "Parse error";
                case AttrCondition::kConversionError:
                    return "Conversion error";
                case AttrCondition::kMissingValue:
                    return "Missing value";
                case AttrCondition::kWrongType:
                    return "Wrong type";
                default:
                    return "INVALID";
            }
        }
    };
}

#endif // TEMPO_UTILS_ATTR_RESULT_H