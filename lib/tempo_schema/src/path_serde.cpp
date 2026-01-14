
#include <tempo_schema/path_serde.h>

tempo_schema::PathAttr::PathAttr(const ComparableResource *resource)
    : AttrSerde<std::filesystem::path>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_schema::PathAttr::writeAttr(AbstractAttrWriter *writer, const std::filesystem::path &path) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putString(path.string());
}

tempo_utils::Status
tempo_schema::PathAttr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, std::filesystem::path &path) const
{
    std::string value;
    auto status = parser->getString(index, value);
    if (status.notOk())
        return status;
    path = std::filesystem::path(value);
    return {};
}
