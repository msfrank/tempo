#ifndef TEMPO_UTILS_URL_PATH_H
#define TEMPO_UTILS_URL_PATH_H

#include <filesystem>
#include <string>
#include <vector>

#include <absl/hash/hash.h>

#include "url_types.h"

namespace tempo_utils {

    class UrlPathPart {
    public:
        UrlPathPart();
        explicit UrlPathPart(const char *part);
        explicit UrlPathPart(std::string_view part);
        UrlPathPart(const UrlPathPart &other);

        bool isValid() const;

        std::string getPart() const;
        std::string_view partView() const;
        std::string decode() const;

        static UrlPathPart curr();
        static UrlPathPart parent();

    private:
        std::string m_part;
    };

    class UrlPath {
    public:
        UrlPath();
        UrlPath(const UrlPath &other);

        bool isValid() const;
        bool isEmpty() const;
        bool isAbsolute() const;

        int numParts() const;
        UrlPathPart getPart(int index) const;
        std::string_view partView(int index) const;
        UrlPathPart getHead() const;
        std::string_view headView() const;
        UrlPathPart getLast() const;
        std::string_view lastView() const;

        UrlPath getInit() const;
        UrlPath getTail() const;

        bool isParentOf(const UrlPath &other) const;
        bool isChildOf(const UrlPath &other) const;
        bool isAncestorOf(const UrlPath &other) const;
        bool isDescendentOf(const UrlPath &other) const;

        std::string_view pathView() const;

        UrlPath traverse(const UrlPathPart &part);

        std::string toString() const;
        std::filesystem::path toFilesystemPath(const std::filesystem::path &relativeBase) const;

        bool operator==(const UrlPath &other) const;
        bool operator!=(const UrlPath &other) const;

        static UrlPath fromString(std::string_view s);

        template <typename H>
        friend H AbslHashValue(H h, const UrlPath &path) {
            auto view = path.pathView();
            return H::combine_contiguous(std::move(h), view.data(), view.size());
        }

    private:
        std::shared_ptr<internal::UrlData> m_priv;
//        std::vector<std::pair<tu_int16,tu_int16>> m_parts;

        explicit UrlPath(std::shared_ptr<internal::UrlData> priv);
//        UrlPath(
//            std::shared_ptr<internal::UrlData> priv,
//            std::vector<std::pair<tu_int16,tu_int16>> &&parts);

        friend class Url;

    public:
        template<class... Args>
        UrlPath traverse(const UrlPathPart &part, Args... args)
        {
            auto path = traverse(part);
            return path.traverse(args...);
        }
    };
}

#endif // TEMPO_UTILS_URL_PATH_H
