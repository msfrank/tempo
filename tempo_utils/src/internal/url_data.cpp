
#include <tempo_utils/internal/url_data.h>

tempo_utils::internal::UrlData::UrlData(ada::url_aggregator &&uri)
    : uri(std::move(uri))
{
}