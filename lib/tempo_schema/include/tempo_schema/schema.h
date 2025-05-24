#ifndef TEMPO_UTILS_SCHEMA_H
#define TEMPO_UTILS_SCHEMA_H

#include <absl/strings/string_view.h>

#include "schema_resource.h"

namespace tempo_schema {

    template<class NsType, class IdType>
    class SchemaVocabulary {

    public:
        constexpr SchemaVocabulary(
            const NsType *ns,
            const SchemaResource<NsType,IdType> * const *resources,
            std::size_t numResources)
            : m_ns(ns), m_resources(resources), m_numResources(numResources) {};
        template<std::size_t numResources>
        constexpr SchemaVocabulary(
            const NsType *ns,
            const std::array<const SchemaResource<NsType,IdType> *, numResources> *resources)
            : m_ns(ns), m_resources(resources->data()), m_numResources(numResources) {};

        constexpr NsType const *getNs() const { return m_ns; }
        constexpr SchemaResource<NsType,IdType> const *getResource(int idValue) const
        {
            if (idValue < 0 || m_numResources <= idValue)
                return nullptr;
            return m_resources[idValue];
        }
        constexpr SchemaResource<NsType,IdType> const *getResource(IdType idType) const
        {
            return getResource(static_cast<int>(idType));
        }
        constexpr SchemaResource<NsType,IdType> const *getResource(std::string_view name) const
        {
            for (int i = 0; i < m_numResources; i++) {
                const auto *resource = m_resources[i];
                if (std::string_view(resource->getName()) == name)
                    return resource;
            }
            return nullptr;
        }
        constexpr int numResources() const { return m_numResources; }

    private:
        const NsType *m_ns;
        const SchemaResource<NsType,IdType> * const *m_resources;
        std::size_t m_numResources;
    };
}

#endif // TEMPO_UTILS_SCHEMA_H