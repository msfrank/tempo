
#include <tempo_utils/url_serde.h>

tempo_utils::UrlAttr::UrlAttr(const ComparableResource *resource)
    : AttrSerde<Url>(resource)
{
}

tempo_utils::Result<tu_uint32>
tempo_utils::UrlAttr::writeAttr(AbstractAttrWriter *writer, const Url &url) const
{
    TU_ASSERT (writer != nullptr);
    return writer->putString(url.toString());
}

tempo_utils::Status
tempo_utils::UrlAttr::parseAttr(tu_uint32 index, AbstractAttrParser *parser, Url &url) const
{
    std::string value;
    auto status = parser->getString(index, value);
    if (status.notOk())
        return status;
    url = Url::fromString(value);
    return AttrStatus::ok();
}

tempo_utils::Status
tempo_utils::UrlAttr::validateAttr(tu_uint32 index, AbstractAttrParser *parser) const
{
    std::string value;
    auto status = parser->getString(index, value);
    if (status.notOk())
        return status;
    return AttrStatus::ok();
}

std::string
tempo_utils::UrlAttr::toString(tu_uint32 index, AbstractAttrParser *parser) const
{
    std::string value;
    auto status = parser->getString(index, value);
    if (status.notOk())
        return "???";
    auto url = Url::fromString(value);
    if (!url.isValid())
        return "???";
    return url.toString();
}
