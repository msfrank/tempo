#ifndef TEMPO_UTILS_URL_ORIGIN_H
#define TEMPO_UTILS_URL_ORIGIN_H

#include <string>

#include "url_types.h"

namespace tempo_utils {

    class UrlOrigin {
    public:
        UrlOrigin();
        UrlOrigin(const UrlOrigin &other);

        bool isValid() const;
        bool isEmpty() const;

        bool hasScheme() const;
        bool hasHost() const;
        bool hasPort() const;

        std::string getScheme() const;
        std::string_view schemeView() const;
        std::string getHost() const;
        std::string_view hostView() const;
        std::string getPort() const;
        std::string_view portView() const;
        std::string getHostAndPort() const;
        std::string_view hostAndPortView() const;

        std::string toString() const;

        bool operator==(const UrlOrigin &other) const;
        bool operator!=(const UrlOrigin &other) const;

    private:
        std::shared_ptr<internal::UrlData> m_priv;

        explicit UrlOrigin(std::shared_ptr<internal::UrlData> priv);

        friend class Url;
    };

}

#endif // TEMPO_UTILS_URL_ORIGIN_H
