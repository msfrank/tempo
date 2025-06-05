#ifndef TEMPO_SECURITY_SECURITY_RESULT_H
#define TEMPO_SECURITY_SECURITY_RESULT_H

#include <string>

#include <fmt/core.h>
#include <fmt/format.h>

#include <tempo_utils/result.h>
#include <tempo_utils/status.h>

namespace tempo_security {

    constexpr const char *kTempoSecurityStatusNs = "dev.zuri.ns:tempo-security-status-1";

    enum class SecurityCondition {
        kParseError,
        kMissingCertificate,
        kMissingPrivateKey,
        kMissingCertificateSigningRequest,
        kMissingTrustStore,
        kInvalidCertificate,
        kInvalidPrivateKey,
        kInvalidCertificateSigningRequest,
        kInvalidTrustStore,
        kSigningFailure,
        kVerificationFailure,
        kSecurityInvariant,
    };

    class SecurityStatus : public tempo_utils::TypedStatus<SecurityCondition> {
    public:
        using TypedStatus::TypedStatus;
        static SecurityStatus ok();
        static bool convert(SecurityStatus &dstStatus, const tempo_utils::Status &srcStatus);

    private:
        SecurityStatus(tempo_utils::StatusCode statusCode, std::shared_ptr<const tempo_utils::Detail> detail);

    public:
        /**
          *
          * @param condition
          * @param message
          * @return
          */
        static SecurityStatus forCondition(
            SecurityCondition condition,
            std::string_view message)
        {
            return SecurityStatus(condition, message);
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
        static SecurityStatus forCondition(
            SecurityCondition condition,
            fmt::string_view messageFmt = {},
            Args... messageArgs) {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            return SecurityStatus(condition, message);
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
        static SecurityStatus forCondition(
            SecurityCondition condition,
            tempo_utils::TraceId traceId,
            tempo_utils::SpanId spanId,
            fmt::string_view messageFmt = {},
            Args... messageArgs) {
            auto message = fmt::vformat(messageFmt, fmt::make_format_args(messageArgs...));
            return ConfigStatus(condition, message, traceId, spanId);
        }
    };

    template<typename T>
    class SecurityResult : public tempo_utils::TypedResult<T,SecurityStatus> {
    public:
        SecurityResult() : tempo_utils::TypedResult<T, SecurityStatus>() {};
        SecurityResult(const T &result) : tempo_utils::TypedResult<T, SecurityStatus>(result) {};
        SecurityResult(const SecurityStatus &status) : tempo_utils::TypedResult<T, SecurityStatus>(status) {};
    };
}

namespace tempo_utils {

    template<>
    struct StatusTraits<tempo_security::SecurityStatus> {
        using ConditionType = tempo_security::SecurityCondition;
        static bool convert(tempo_security::SecurityStatus &dstStatus, const tempo_utils::Status &srcStatus)
        {
            return tempo_security::SecurityStatus::convert(dstStatus, srcStatus);
        }
    };

    template<>
    struct ConditionTraits<tempo_security::SecurityCondition> {
        using StatusType = tempo_security::SecurityStatus;
        static constexpr const char *condition_namespace() { return tempo_security::kTempoSecurityStatusNs; }
        static constexpr StatusCode make_status_code(tempo_security::SecurityCondition condition)
        {
            switch (condition) {
                case tempo_security::SecurityCondition::kParseError:
                    return tempo_utils::StatusCode::kInvalidArgument;
                case tempo_security::SecurityCondition::kMissingCertificate:
                    return tempo_utils::StatusCode::kInvalidArgument;
                case tempo_security::SecurityCondition::kMissingPrivateKey:
                    return tempo_utils::StatusCode::kInvalidArgument;
                case tempo_security::SecurityCondition::kMissingCertificateSigningRequest:
                    return tempo_utils::StatusCode::kInvalidArgument;
                case tempo_security::SecurityCondition::kMissingTrustStore:
                    return tempo_utils::StatusCode::kInvalidArgument;
                case tempo_security::SecurityCondition::kInvalidCertificate:
                    return tempo_utils::StatusCode::kInvalidArgument;
                case tempo_security::SecurityCondition::kInvalidPrivateKey:
                    return tempo_utils::StatusCode::kInvalidArgument;
                case tempo_security::SecurityCondition::kInvalidCertificateSigningRequest:
                    return tempo_utils::StatusCode::kInvalidArgument;
                case tempo_security::SecurityCondition::kInvalidTrustStore:
                    return tempo_utils::StatusCode::kInvalidArgument;
                case tempo_security::SecurityCondition::kSigningFailure:
                    return tempo_utils::StatusCode::kInternal;
                case tempo_security::SecurityCondition::kVerificationFailure:
                case tempo_security::SecurityCondition::kSecurityInvariant:
                    return tempo_utils::StatusCode::kInternal;
                default:
                    return tempo_utils::StatusCode::kUnknown;
            }
        };
        static constexpr const char *make_error_message(tempo_security::SecurityCondition condition)
        {
            switch (condition) {
                case tempo_security::SecurityCondition::kParseError:
                    return "Parse error";
                case tempo_security::SecurityCondition::kMissingCertificate:
                    return "Missing certificate";
                case tempo_security::SecurityCondition::kMissingPrivateKey:
                    return "Missing private key";
                case tempo_security::SecurityCondition::kMissingCertificateSigningRequest:
                    return "Missing certificate signing request";
                case tempo_security::SecurityCondition::kMissingTrustStore:
                    return "Missing trust store";
                case tempo_security::SecurityCondition::kInvalidCertificate:
                    return "Invalid certificate";
                case tempo_security::SecurityCondition::kInvalidPrivateKey:
                    return "Invalid private key";
                case tempo_security::SecurityCondition::kInvalidCertificateSigningRequest:
                    return "Invalid certificate signing request";
                case tempo_security::SecurityCondition::kInvalidTrustStore:
                    return "Invalid trust store";
                case tempo_security::SecurityCondition::kSigningFailure:
                    return "Signing failure";
                case tempo_security::SecurityCondition::kVerificationFailure:
                    return "Verification failure";
                case tempo_security::SecurityCondition::kSecurityInvariant:
                    return "Security invariant";
                default:
                    return "INVALID";
            }
        }
    };
}

#endif // TEMPO_SECURITY_SECURITY_RESULT_H