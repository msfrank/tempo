#ifndef TEMPO_UTILS_URL_H
#define TEMPO_UTILS_URL_H

#include <string>
#include <vector>

#include <absl/hash/hash.h>

#include "integer_types.h"
#include "log_message.h"
#include "url_authority.h"
#include "url_origin.h"
#include "url_path.h"
#include "url_types.h"

namespace tempo_utils {

    /**
     * A uniform resource locator (URL).
     */
    class Url {
    public:
        Url();
        Url(const Url &other);
        Url(Url &&other) noexcept;

        Url& operator=(const Url &other);
        Url& operator=(Url &&other) noexcept;

        bool isValid() const;
        bool isAbsolute() const;
        bool isAuthority() const;
        bool isRelative() const;
        bool isFragment() const;

        bool hasScheme() const;
        bool hasOrigin() const;
        bool hasAuthority() const;
        bool hasPath() const;
        bool hasQuery() const;
        bool hasFragment() const;

        bool isKnownScheme() const;
        KnownUrlScheme getKnownScheme() const;

        std::string getScheme() const;
        std::string_view schemeView() const;
        std::string getHost() const;
        std::string_view hostView() const;
        std::string getPort() const;
        std::string_view portView() const;
        std::string getHostAndPort() const;
        std::string_view hostAndPortView() const;
        std::string getUsername() const;
        std::string_view usernameView() const;
        std::string getPassword() const;
        std::string_view passwordView() const;
        std::string getPath() const;
        std::string_view pathView() const;
        std::string getQuery() const;
        std::string_view queryView() const;
        std::string getFragment() const;
        std::string_view fragmentView() const;

        std::string_view uriView() const;

        Url traverse(const UrlPathPart &part);

        UrlOrigin toOrigin() const;
        UrlAuthority toAuthority() const;
        UrlPath toPath() const;

        std::string toString() const;
        std::filesystem::path toFilesystemPath(const std::filesystem::path &relativeBase) const;

        bool validate(tu_uint16 validations = 0) const;

        bool operator==(const Url &other) const;
        bool operator!=(const Url &other) const;

        static Url fromString(std::string_view s);

        static Url fromAbsolute(
            std::string_view scheme,
            std::string_view authority,
            std::string_view path = {},
            std::string_view query = {},
            std::string_view fragment = {});

        static Url fromAbsolute(
            std::string_view scheme,
            const UrlAuthority &authority,
            std::string_view path = {},
            std::string_view query = {},
            std::string_view fragment = {});

        static Url fromAuthority(
            std::string_view authority,
            std::string_view path = {},
            std::string_view query = {},
            std::string_view fragment = {});

        static Url fromAuthority(
            const UrlAuthority &authority,
            std::string_view path = {},
            std::string_view query = {},
            std::string_view fragment = {});

        static Url fromOrigin(
            const UrlOrigin &origin,
            std::string_view path = {},
            std::string_view query = {},
            std::string_view fragment = {});

        static Url fromOrigin(
            std::string_view origin,
            std::string_view path = {},
            std::string_view query = {},
            std::string_view fragment = {});

        static Url fromRelative(
            std::string_view path,
            std::string_view query = {},
            std::string_view fragment = {});

        static Url fromFragment(std::string_view fragment);

        static Url fromFilesystemPath(const std::filesystem::path &path);

        Url withScheme(std::string_view scheme) const;
        Url withPath(std::string_view path) const;
        Url withQuery(std::string_view query) const;
        Url withFragment(std::string_view fragment) const;

        template <typename H>
        friend H AbslHashValue(H h, const tempo_utils::Url &uri) {
            auto view = uri.uriView();
            return H::combine_contiguous(std::move(h), view.data(), view.size());
        }

    private:
        std::shared_ptr<internal::UrlData> m_priv;

        explicit Url(std::shared_ptr<internal::UrlData> priv);
    };

    LogMessage&& operator<<(LogMessage &&message, const Url &uri);
}

#endif // TEMPO_UTILS_URL_H
