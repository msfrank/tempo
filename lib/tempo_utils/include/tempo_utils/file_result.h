#ifndef TEMPO_UTILS_FILE_RESULT_H
#define TEMPO_UTILS_FILE_RESULT_H

#include "status.h"

namespace tempo_utils {

    constexpr const char *kTempoUtilsFileStatusNs = "dev.zuri.ns:tempo-utils-file-status-1";

    enum class FileCondition {
        kInvalidName,
        kInvalidTemplate,
        kFileNotFound,
        kParentDirectoryNotFound,
        kAccessDenied,
        kFileExists,
        kMissingBaseDirectory,
        kInvalidBaseDirectory,
        kSystemInvariant,
    };

    class FileStatus : public TypedStatus<FileCondition> {
    public:
        using TypedStatus::TypedStatus;
        static FileStatus ok();
        static bool convert(FileStatus &dstStatus, const tempo_utils::Status &srcStatus);

    private:
        FileStatus(tempo_utils::StatusCode statusCode, std::shared_ptr<const tempo_utils::Detail> detail);

    public:
        /**
         *
         * @param condition
         * @param message
         * @return
         */
        static FileStatus forCondition(
            FileCondition condition,
            std::string_view message)
        {
            return FileStatus(condition, message);
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
        static FileStatus forCondition(
            FileCondition condition,
            fmt::string_view messageFmt = {},
            Args... messageArgs)
        {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            return FileStatus(condition, message);
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
        static FileStatus forCondition(
            FileCondition condition,
            tempo_utils::TraceId traceId,
            tempo_utils::SpanId spanId,
            fmt::string_view messageFmt = {},
            Args... messageArgs)
        {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            return FileStatus(condition, message, traceId, spanId);
        }
    };

    template<>
    struct StatusTraits<FileStatus> {
        using ConditionType = FileCondition;
        static bool convert(FileStatus &dstStatus, const tempo_utils::Status &srcStatus)
        {
            return FileStatus::convert(dstStatus, srcStatus);
        }
    };

    template<>
    struct ConditionTraits<FileCondition> {
        using StatusType = FileStatus;
        static constexpr const char *condition_namespace() { return kTempoUtilsFileStatusNs; }
        static constexpr StatusCode make_status_code(FileCondition condition)
        {
            switch (condition) {
                case FileCondition::kInvalidName:
                    return tempo_utils::StatusCode::kInvalidArgument;
                case FileCondition::kInvalidTemplate:
                    return tempo_utils::StatusCode::kInvalidArgument;
                case FileCondition::kFileNotFound:
                    return tempo_utils::StatusCode::kNotFound;
                case FileCondition::kParentDirectoryNotFound:
                    return tempo_utils::StatusCode::kNotFound;
                case FileCondition::kAccessDenied:
                    return tempo_utils::StatusCode::kPermissionDenied;
                case FileCondition::kFileExists:
                    return tempo_utils::StatusCode::kFailedPrecondition;
                case FileCondition::kMissingBaseDirectory:
                    return tempo_utils::StatusCode::kFailedPrecondition;
                case FileCondition::kInvalidBaseDirectory:
                    return tempo_utils::StatusCode::kFailedPrecondition;
                case FileCondition::kSystemInvariant:
                    return tempo_utils::StatusCode::kInternal;
                default:
                    return tempo_utils::StatusCode::kUnknown;
            }
        };
        static constexpr const char *make_error_message(FileCondition condition)
        {
            switch (condition) {
                case FileCondition::kInvalidName:
                    return "Invalid name";
                case FileCondition::kInvalidTemplate:
                    return "Invalid template";
                case FileCondition::kFileNotFound:
                    return "File not found";
                case FileCondition::kParentDirectoryNotFound:
                    return "Parent directory not found";
                case FileCondition::kAccessDenied:
                    return "Access denied";
                case FileCondition::kFileExists:
                    return "File exists";
                case FileCondition::kMissingBaseDirectory:
                    return "Missing base directory";
                case FileCondition::kInvalidBaseDirectory:
                    return "Invalid base directory";
                case FileCondition::kSystemInvariant:
                    return "System invariant";
                default:
                    return "INVALID";
            }
        }
    };
}

#endif // TEMPO_UTILS_FILE_RESULT_H