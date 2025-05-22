#ifndef TEMPO_UTILS_URL_AUTHORITY_H
#define TEMPO_UTILS_URL_AUTHORITY_H

#include <string>

#include "url_types.h"

namespace tempo_utils {

    class UrlAuthority {
    public:
        UrlAuthority();
        UrlAuthority(const UrlAuthority &other);

        bool isValid() const;
        bool isEmpty() const;

        bool hasUsername() const;
        bool hasPassword() const;
        bool hasCredentials() const;
        bool hasHost() const;
        bool hasPort() const;

        std::string getUsername() const;
        std::string_view usernameView() const;
        std::string getPassword() const;
        std::string_view passwordView() const;
        std::string getHost() const;
        std::string_view hostView() const;
        std::string getPort() const;
        std::string_view portView() const;
        std::string getHostAndPort() const;
        std::string_view hostAndPortView() const;

        std::string_view authorityView() const;

        std::string toString() const;

        bool operator==(const UrlAuthority &other) const;
        bool operator!=(const UrlAuthority &other) const;

        static UrlAuthority fromString(std::string_view s);

    private:
        std::shared_ptr<internal::UrlData> m_priv;

        explicit UrlAuthority(std::shared_ptr<internal::UrlData> priv);

        friend class Url;
    };
}

#endif // TEMPO_UTILS_URL_AUTHORITY_H
