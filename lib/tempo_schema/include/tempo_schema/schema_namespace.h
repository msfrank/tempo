#ifndef TEMPO_SCHEMA_SCHEMA_NAMESPACE_H
#define TEMPO_SCHEMA_SCHEMA_NAMESPACE_H

#include <string_view>

namespace tempo_schema {

    class SchemaNs {

    public:
        constexpr explicit SchemaNs(const char *ns) : m_ns(ns) {};
        constexpr char const *getNs() const { return m_ns; };
        constexpr bool operator==(const SchemaNs &other) const
        {
            if (m_ns == nullptr && other.m_ns == nullptr)
                return true;
            return m_ns != nullptr && other.m_ns != nullptr
                && std::string_view(m_ns) == std::string_view(other.m_ns);
        }

        template <typename H>
        friend H AbslHashValue(H h, const SchemaNs &ns) {
            return H::combine(std::move(h), std::string_view{ns.m_ns});
        }

    private:
        char const *m_ns;
    };
}

#endif // TEMPO_SCHEMA_SCHEMA_NAMESPACE_H
