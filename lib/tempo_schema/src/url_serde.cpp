
#include <tempo_schema/url_serde.h>

tempo_schema::UrlAttr::UrlAttr(const ComparableResource *resource)
    : AttrSerde<tempo_utils::Url>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_schema::UrlAttr::writeAttr(AbstractAttrWriter *writer, const tempo_utils::Url &url) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putString(url.toString());
}

tempo_utils::Status
tempo_schema::UrlAttr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, tempo_utils::Url &url) const
{
    std::string value;
    auto status = parser->getString(index, value);
    if (status.notOk())
        return status;
    url = tempo_utils::Url::fromString(value);
    return {};
}