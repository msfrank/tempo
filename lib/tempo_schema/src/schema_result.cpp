
#include <tempo_schema/schema_result.h>

tempo_schema::SchemaStatus::SchemaStatus(
    tempo_utils::StatusCode statusCode,
    std::shared_ptr<const tempo_utils::Detail> detail)
    : tempo_utils::TypedStatus<SchemaCondition>(statusCode, detail)
{
}

bool
tempo_schema::SchemaStatus::convert(SchemaStatus &dstStatus, const tempo_utils::Status &srcStatus)
{
    std::string_view srcNs = srcStatus.getErrorCategory();
    std::string_view dstNs = kTempoSchemaSchemaStatusNs;
    if (srcNs != dstNs)
        return false;
    dstStatus = SchemaStatus(srcStatus.getStatusCode(), srcStatus.getDetail());
    return true;
}