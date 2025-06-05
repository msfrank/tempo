#ifndef TEMPO_UTILS_LIBRARY_RESULT_H
#define TEMPO_UTILS_LIBRARY_RESULT_H

#include "status.h"

namespace tempo_utils {

    constexpr const char *kTempoUtilsLibraryStatusNs = "dev.zuri.ns:tempo-utils-library-status-1";

    enum class LibraryCondition {
        kLibraryInvariant,
    };

    class LibraryStatus : public TypedStatus<LibraryCondition> {
    public:
        using TypedStatus::TypedStatus;
        static LibraryStatus ok();
        static bool convert(LibraryStatus &dstStatus, const tempo_utils::Status &srcStatus);

    private:
        LibraryStatus(tempo_utils::StatusCode statusCode, std::shared_ptr<const tempo_utils::Detail> detail);

    public:
        /**
         *
         * @param condition
         * @param message
         * @return
         */
        static LibraryStatus forCondition(
            LibraryCondition condition,
            std::string_view message)
        {
            return LibraryStatus(condition, message);
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
        static LibraryStatus forCondition(
            LibraryCondition condition,
            fmt::string_view messageFmt = {},
            Args... messageArgs)
        {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            return LibraryStatus(condition, message);
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
        static LibraryStatus forCondition(
            LibraryCondition condition,
            tempo_utils::TraceId traceId,
            tempo_utils::SpanId spanId,
            fmt::string_view messageFmt = {},
            Args... messageArgs)
        {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            return LibraryStatus(condition, message, traceId, spanId);
        }
    };

    template<>
    struct StatusTraits<LibraryStatus> {
        using ConditionType = LibraryCondition;
        static bool convert(LibraryStatus &dstStatus, const tempo_utils::Status &srcStatus)
        {
            return LibraryStatus::convert(dstStatus, srcStatus);
        }
    };

    template<>
    struct ConditionTraits<LibraryCondition> {
        using StatusType = LibraryStatus;
        static constexpr const char *condition_namespace() { return kTempoUtilsLibraryStatusNs; }
        static constexpr StatusCode make_status_code(LibraryCondition condition)
        {
            switch (condition) {
                case LibraryCondition::kLibraryInvariant:
                    return tempo_utils::StatusCode::kInternal;
                default:
                    return tempo_utils::StatusCode::kUnknown;
            }
        };
        static constexpr const char *make_error_message(LibraryCondition condition)
        {
            switch (condition) {
                case LibraryCondition::kLibraryInvariant:
                    return "Library invariant";
                default:
                    return "INVALID";
            }
        }
    };
}

#endif // TEMPO_UTILS_LIBRARY_RESULT_H