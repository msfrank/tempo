#ifndef TEMPO_SCHEMA_SCHEMA_RESULT_H
#define TEMPO_SCHEMA_SCHEMA_RESULT_H

#include <string>

#include <fmt/core.h>
#include <fmt/format.h>

#include <tempo_utils/result.h>
#include <tempo_utils/status.h>

namespace tempo_schema {
    constexpr const char *kTempoSchemaSchemaStatusNs = "dev.zuri.ns:tempo-schema-status-1";

    enum class SchemaCondition {
        kParseError,
        kConversionError,
        kMissingValue,
        kWrongType,
    };

    class SchemaStatus : public tempo_utils::TypedStatus<SchemaCondition> {
    public:
        using TypedStatus::TypedStatus;
        static bool convert(SchemaStatus &dstStatus, const tempo_utils::Status &srcStatus);

    private:
        SchemaStatus(tempo_utils::StatusCode statusCode, std::shared_ptr<const tempo_utils::Detail> detail);

    public:
        /**
          *
          * @param condition
          * @param message
          * @return
          */
        static SchemaStatus forCondition(
            SchemaCondition condition,
            std::string_view message)
        {
            return SchemaStatus(condition, message);
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
        static SchemaStatus forCondition(
            SchemaCondition condition,
            fmt::string_view messageFmt = {},
            Args... messageArgs)
        {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            return SchemaStatus(condition, message);
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
        static SchemaStatus forCondition(
            SchemaCondition condition,
            tempo_utils::TraceId traceId,
            tempo_utils::SpanId spanId,
            fmt::string_view messageFmt = {},
            Args... messageArgs)
        {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            return SchemaStatus(condition, message, traceId, spanId);
        }
    };
}

namespace tempo_utils {
    template<>
    struct StatusTraits<tempo_schema::SchemaStatus> {
        using ConditionType = tempo_schema::SchemaCondition;
        static bool convert(tempo_schema::SchemaStatus &dstStatus, const tempo_utils::Status &srcStatus)
        {
            return tempo_schema::SchemaStatus::convert(dstStatus, srcStatus);
        }
    };

    template<>
    struct ConditionTraits<tempo_schema::SchemaCondition> {
        using StatusType = tempo_schema::SchemaStatus;
        static constexpr const char *condition_namespace() { return tempo_schema::kTempoSchemaSchemaStatusNs; }
        static constexpr tempo_utils::StatusCode make_status_code(tempo_schema::SchemaCondition condition)
        {
            switch (condition) {
                case tempo_schema::SchemaCondition::kParseError:
                    return tempo_utils::StatusCode::kInternal;
                case tempo_schema::SchemaCondition::kConversionError:
                    return tempo_utils::StatusCode::kInvalidArgument;
                case tempo_schema::SchemaCondition::kMissingValue:
                    return tempo_utils::StatusCode::kNotFound;
                case tempo_schema::SchemaCondition::kWrongType:
                    return tempo_utils::StatusCode::kInvalidArgument;
                default:
                    return tempo_utils::StatusCode::kUnknown;
            }
        };
        static constexpr const char *make_error_message(tempo_schema::SchemaCondition condition)
        {
            switch (condition) {
                case tempo_schema::SchemaCondition::kParseError:
                    return "Parse error";
                case tempo_schema::SchemaCondition::kConversionError:
                    return "Conversion error";
                case tempo_schema::SchemaCondition::kMissingValue:
                    return "Missing value";
                case tempo_schema::SchemaCondition::kWrongType:
                    return "Wrong type";
                default:
                    return "INVALID";
            }
        }
    };
}

#endif // TEMPO_SCHEMA_SCHEMA_RESULT_H