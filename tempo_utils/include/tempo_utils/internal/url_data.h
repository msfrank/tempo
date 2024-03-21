#ifndef TEMPO_UTILS_INTERNAL_URL_DATA_H
#define TEMPO_UTILS_INTERNAL_URL_DATA_H

#include <boost/url.hpp>

namespace tempo_utils::internal {

    struct UrlData {
        std::string data;
        boost::url_view url;
    };
}

#endif // TEMPO_UTILS_INTERNAL_URL_DATA_H
