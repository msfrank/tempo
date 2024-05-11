
#include <tempo_utils/log_stream.h>
#include <tempo_utils/status.h>

static std::array<const char *, (int) tempo_utils::StatusCode::NUM_CONDITIONS> kStatusCodeStrings = {
    "Ok",
    "Cancelled",
    "InvalidArgument",
    "DeadlineExceeded",
    "NotFound",
    "AlreadyExists",
    "PermissionDenied",
    "Unauthenticated",
    "ResourceExhausted",
    "FailedPrecondition",
    "Aborted",
    "Unavailable",
    "OutOfRange",
    "Unimplemented",
    "Internal",
    "Unknown",
};

const char *
tempo_utils::status_code_to_string(StatusCode condition)
{
    int index = static_cast<int>(condition);
    if (0 <= index && index < static_cast<int>(StatusCode::NUM_CONDITIONS))
        return kStatusCodeStrings.at(index);
    return nullptr;
}

tempo_utils::Detail::Detail(
    const char *errorCategory,
    int errorCode,
    std::string_view message,
    TraceId traceId,
    SpanId spanId)
    : m_errorCategory(errorCategory),
      m_errorCode(errorCode),
      m_message(message),
      m_traceId(traceId),
      m_spanId(spanId)
{
}

const char *
tempo_utils::Detail::getErrorCategory() const
{
    return m_errorCategory;
}

int
tempo_utils::Detail::getErrorCode() const
{
    return m_errorCode;
}

std::string_view
tempo_utils::Detail::getMessage() const
{
    return m_message;
}

tempo_utils::TraceId
tempo_utils::Detail::getTraceId() const
{
    return m_traceId;
}

tempo_utils::SpanId
tempo_utils::Detail::getSpanId() const
{
    return m_spanId;
}

std::string
tempo_utils::Detail::toString() const
{
    return m_message;
}

tempo_utils::Status::Status()
    : m_statusCode(StatusCode::kOk),
      m_detail()
{
}

tempo_utils::Status::Status(StatusCode statusCode, std::string_view message)
    : m_statusCode(statusCode)
{
    if (!message.empty()) {
        m_detail = std::make_shared<Detail>(nullptr, -1, message, TraceId(), SpanId());
    }
}

tempo_utils::Status::Status(StatusCode statusCode, std::shared_ptr<const Detail> detail)
    : m_statusCode(statusCode),
      m_detail(detail)
{
    TU_ASSERT (detail != nullptr);
}

tempo_utils::Status::Status(
    StatusCode statusCode,
    const char *errorCategory,
    int errorCode,
    std::string_view message,
    TraceId traceId,
    SpanId spanId)
    : m_statusCode(statusCode),
      m_detail(std::make_shared<Detail>(errorCategory, errorCode, message, traceId, spanId))
{
}

tempo_utils::Status::Status(const Status &other)
    : m_statusCode(other.m_statusCode),
      m_detail(other.m_detail)
{
}

tempo_utils::Status::Status(Status &&other) noexcept
{
    m_statusCode = other.m_statusCode;
    m_detail.swap(other.m_detail);
}

tempo_utils::Status&
tempo_utils::Status::operator=(const Status &other)
{
    m_statusCode = other.m_statusCode;
    m_detail = other.m_detail;
    return *this;
}

tempo_utils::Status&
tempo_utils::Status::operator=(Status &&other) noexcept
{
    if (this != &other) {
        m_statusCode = other.m_statusCode;
        m_detail.swap(other.m_detail);
    }
    return *this;
}

bool
tempo_utils::Status::isOk() const
{
    return m_statusCode == StatusCode::kOk;
}

bool
tempo_utils::Status::notOk() const
{
    return m_statusCode != StatusCode::kOk;
}

bool
tempo_utils::Status::isRetryable() const
{
    switch (m_statusCode) {
        case StatusCode::kOk:
        case StatusCode::kCancelled:
        case StatusCode::kDeadlineExceeded:
        case StatusCode::kResourceExhausted:
        case StatusCode::kAborted:
        case StatusCode::kUnavailable:
            return true;
        default:
            return false;
    }
}

tempo_utils::StatusCode
tempo_utils::Status::getStatusCode() const
{
    return m_statusCode;
}

std::string_view
tempo_utils::Status::getErrorCategory() const
{
    if (m_detail == nullptr)
        return {};
    return m_detail->getErrorCategory();
}

int
tempo_utils::Status::getErrorCode() const
{
    if (m_detail == nullptr)
        return -1;
    return m_detail->getErrorCode();
}

std::string_view
tempo_utils::Status::getMessage() const
{
    if (m_detail == nullptr)
        return {};
    return m_detail->getMessage();
}

std::string
tempo_utils::Status::toString() const
{
    if (m_detail)
        return m_detail->toString();
    const auto *message = status_code_to_string(m_statusCode);
    if (message)
        return std::string(message);
    return {};
}

bool
tempo_utils::Status::isTraced() const
{
    if (!m_detail)
        return false;
    return m_detail->getTraceId().isValid() && m_detail->getSpanId().isValid();
}

tempo_utils::TraceId
tempo_utils::Status::getTraceId() const
{
    if (m_detail)
        return m_detail->getTraceId();
    return {};
}

tempo_utils::SpanId
tempo_utils::Status::getSpanId() const
{
    if (m_detail)
        return m_detail->getSpanId();
    return {};
}

std::shared_ptr<const tempo_utils::Detail>
tempo_utils::Status::getDetail() const
{
    return m_detail;
}

void
tempo_utils::Status::andThrow() const
{
    throw StatusException(*this);
}

void
tempo_utils::Status::orThrow() const
{
    if (m_statusCode != StatusCode::kOk)
        throw StatusException(*this);
}

tempo_utils::LogMessage&&
tempo_utils::operator<<(tempo_utils::LogMessage &&message, const Status &status)
{
    std::forward<tempo_utils::LogMessage>(message) << status.toString();
    return std::move(message);
}

tempo_utils::StatusException::StatusException(Status status) noexcept
    : m_status(status)
{
}

tempo_utils::Status
tempo_utils::StatusException::getStatus() const
{
    return m_status;
}

const char *
tempo_utils::StatusException::what() const noexcept
{
    return m_status.getMessage().data();
}

tempo_utils::GenericStatus::GenericStatus(
    tempo_utils::StatusCode statusCode,
    std::shared_ptr<const tempo_utils::Detail> detail)
    : tempo_utils::TypedStatus<GenericCondition>(statusCode, detail)
{
}

tempo_utils::GenericStatus
tempo_utils::GenericStatus::ok()
{
    return GenericStatus();
}

bool
tempo_utils::GenericStatus::convert(GenericStatus &dstStatus, const Status &srcStatus)
{
    std::string_view srcNs = srcStatus.getErrorCategory();
    std::string_view dstNs = kTempoUtilsGenericStatusNs.getNs();
    if (srcNs != dstNs)
        return false;
    dstStatus = GenericStatus(srcStatus.getStatusCode(), srcStatus.getDetail());
    return true;
}

/**
 * Shortcut to return a GenericStatus containing the kUnimplemented condition.
 *
 * @param reason A string containing the Status message
 * @return A Status containing the kUnimplemented condition
 */
tempo_utils::Status
tempo_utils::unimplemented(std::string_view reason)
{
    return GenericStatus::forCondition(GenericCondition::kUnimplemented, reason);
}

/**
 * Shortcut to return a GenericStatus containing the kInternalViolation condition.
 *
 * @param reason A string containing the Status message
 * @return A Status containing the kInternalViolation condition
 */
tempo_utils::Status
tempo_utils::internalViolation(std::string_view reason)
{
    return GenericStatus::forCondition(GenericCondition::kInternalViolation, reason);
}