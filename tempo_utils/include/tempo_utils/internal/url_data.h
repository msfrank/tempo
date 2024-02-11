#ifndef TEMPO_UTILS_INTERNAL_URL_DATA_H
#define TEMPO_UTILS_INTERNAL_URL_DATA_H

#include <ada.h>
#include <ada/url_aggregator-inl.h>

namespace tempo_utils::internal {

    struct UrlData {
        const ada::url_aggregator uri;
        UrlData(ada::url_aggregator &&uri);
    };
}

#endif // TEMPO_UTILS_INTERNAL_URL_DATA_H
