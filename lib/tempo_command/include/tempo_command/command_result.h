#ifndef TEMPO_COMMAND_COMMAND_RESULT_H
#define TEMPO_COMMAND_COMMAND_RESULT_H

#include <tempo_utils/result.h>
#include <tempo_utils/status.h>

namespace tempo_command {

    constexpr const char *kTempoCommandStatusNs = "dev.zuri.ns:tempo-command-status-1";

    enum class CommandCondition {
        kUnknownSubcommand,
        kUnknownCommandOption,
        kMissingCommandOption,
        kInvalidConfiguration,
        kCommandError,
        kInvalidToken,
        kUnexpectedTokenEnd,
        kCommandInvariant,
        kHelpRequested,
        kVersionRequested,
    };

    class CommandStatus : public tempo_utils::TypedStatus<CommandCondition> {
    public:
        using TypedStatus::TypedStatus;
        static bool convert(CommandStatus &dstStatus, const tempo_utils::Status &srcStatus);

    private:
        CommandStatus(tempo_utils::StatusCode statusCode, std::shared_ptr<const tempo_utils::Detail> detail);

    public:
        /**
          *
          * @param condition
          * @param message
          * @return
          */
        static CommandStatus forCondition(
            CommandCondition condition,
            std::string_view message)
        {
            return CommandStatus(condition, message);
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
        static CommandStatus forCondition(
            CommandCondition condition,
            fmt::string_view messageFmt = {},
            Args... messageArgs) {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            return CommandStatus(condition, message);
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
        static CommandStatus forCondition(
            CommandCondition condition,
            tempo_utils::TraceId traceId,
            tempo_utils::SpanId spanId,
            fmt::string_view messageFmt = {},
            Args... messageArgs) {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            return CommandStatus(condition, message, traceId, spanId);
        }
    };

    template<class T>
    class CommandResult : public tempo_utils::TypedResult<T, CommandStatus> {
    public:
        CommandResult() : tempo_utils::TypedResult<T, CommandStatus>() {};
        CommandResult(const T &result) : tempo_utils::TypedResult<T, CommandStatus>(result) {};
        CommandResult(const CommandStatus &status) : tempo_utils::TypedResult<T, CommandStatus>(status) {};
    };
}

namespace tempo_utils {

    template<>
    struct StatusTraits<tempo_command::CommandStatus> {
        using ConditionType = tempo_command::CommandCondition;
        static bool convert(tempo_command::CommandStatus &dstStatus, const tempo_utils::Status &srcStatus)
        {
            return tempo_command::CommandStatus::convert(dstStatus, srcStatus);
        }
    };

    template<>
    struct ConditionTraits<tempo_command::CommandCondition> {
        using StatusType = tempo_command::CommandStatus;
        static constexpr const char *condition_namespace() { return tempo_command::kTempoCommandStatusNs; }
        static constexpr StatusCode make_status_code(tempo_command::CommandCondition condition)
        {
            switch (condition) {
                case tempo_command::CommandCondition::kInvalidConfiguration:
                    return tempo_utils::StatusCode::kInvalidArgument;
                case tempo_command::CommandCondition::kUnknownSubcommand:
                    return tempo_utils::StatusCode::kInvalidArgument;
                case tempo_command::CommandCondition::kUnknownCommandOption:
                    return tempo_utils::StatusCode::kInvalidArgument;
                case tempo_command::CommandCondition::kMissingCommandOption:
                    return tempo_utils::StatusCode::kInvalidArgument;
                case tempo_command::CommandCondition::kCommandError:
                    return tempo_utils::StatusCode::kInternal;
                case tempo_command::CommandCondition::kInvalidToken:
                    return tempo_utils::StatusCode::kInternal;
                case tempo_command::CommandCondition::kUnexpectedTokenEnd:
                    return tempo_utils::StatusCode::kInternal;
                case tempo_command::CommandCondition::kCommandInvariant:
                    return tempo_utils::StatusCode::kInternal;
                case tempo_command::CommandCondition::kHelpRequested:
                    return tempo_utils::StatusCode::kUnknown;
                case tempo_command::CommandCondition::kVersionRequested:
                    return tempo_utils::StatusCode::kUnknown;
                default:
                    return tempo_utils::StatusCode::kUnknown;
            }
        };
        static constexpr const char *make_error_message(tempo_command::CommandCondition condition)
        {
            switch (condition) {
                case tempo_command::CommandCondition::kInvalidConfiguration:
                    return "Invalid configuration";
                case tempo_command::CommandCondition::kUnknownSubcommand:
                    return "Unknown subcommand";
                case tempo_command::CommandCondition::kUnknownCommandOption:
                    return "Unknown command option";
                case tempo_command::CommandCondition::kMissingCommandOption:
                    return "Missing command option";
                case tempo_command::CommandCondition::kCommandError:
                    return "Command failure";
                case tempo_command::CommandCondition::kInvalidToken:
                    return "Invalid token";
                case tempo_command::CommandCondition::kUnexpectedTokenEnd:
                    return "Unexpected token end";
                case tempo_command::CommandCondition::kCommandInvariant:
                    return "Command invariant";
                case tempo_command::CommandCondition::kHelpRequested:
                    return "Help requested";
                case tempo_command::CommandCondition::kVersionRequested:
                    return "Version requested";
                default:
                    return "INVALID";
            }
        }
    };
}

#endif // TEMPO_COMMAND_COMMAND_RESULT_H