#ifndef TEMPO_UTILS_SCHEMA_H
#define TEMPO_UTILS_SCHEMA_H

#include <absl/strings/string_view.h>

#include "integer_types.h"

namespace tempo_utils {

    class SchemaNs {

    public:
        constexpr SchemaNs(const char *ns) : m_ns(ns) {};
        constexpr char const *getNs() const { return m_ns; };
        constexpr bool operator==(const SchemaNs &other) const
        {
            if (m_ns == nullptr && other.m_ns == nullptr)
                return true;
            return m_ns != nullptr && other.m_ns != nullptr
                && std::string_view(m_ns) == std::string_view(other.m_ns);
        }

    private:
        char const *m_ns;
    };

    class SchemaId {

    public:
        constexpr SchemaId(tu_uint32 id) : m_id(id) {};
        constexpr tu_uint32 getId() const { return m_id; };

    private:
        tu_uint32 m_id;
    };

    inline bool operator==(const SchemaId &lhs, const SchemaId &rhs)
    {
        return lhs.getId() == rhs.getId();
    }

    inline bool operator!=(const SchemaId &lhs, const SchemaId &rhs)
    {
        return lhs.getId() != rhs.getId();
    }

    enum class ResourceType {
        kClass,
        kProperty,
        kEnum,
        kLiteral,
    };

    class ComparableResource {

    public:
        constexpr ComparableResource(ResourceType type, const char *uri, tu_uint32 id, const char *name)
            : m_type(type), m_uri(uri), m_id(id), m_name(name) {};

        constexpr ResourceType const getType() const { return m_type; };
        constexpr char const *getNsUrl() const { return m_uri; };
        constexpr tu_uint32 getIdValue() const { return m_id; };
        constexpr char const *getName() const { return m_name; };
        constexpr bool operator==(const ComparableResource &other) const
        {
            if (m_type != other.m_type)
                return false;
            if (m_uri == nullptr && other.m_uri == nullptr)
                return true;
            return m_uri != nullptr && other.m_uri != nullptr
                   && m_id == other.m_id
                   && std::string_view(m_uri) == std::string_view(other.m_uri);
        };

    private:
        ResourceType m_type;
        const char *m_uri;
        const tu_uint32 m_id;
        const char *m_name;
    };

    template<class NsType, class IdType>
    class SchemaResource : public ComparableResource {

    public:
        constexpr SchemaResource(ResourceType type, const NsType *ns, IdType id, const char *name)
            : ComparableResource(type, ns->getNs(), static_cast<tu_uint32>(id), name), m_ns(ns) {};

        constexpr NsType const *getNs() const { return m_ns; };
        constexpr IdType getId() const { return static_cast<IdType>(ComparableResource::getIdValue()); };

    private:
        const NsType *m_ns;
    };

    template<class NsType, class IdType>
    class SchemaClass : public SchemaResource<NsType,IdType> {

    public:
        constexpr SchemaClass(
            const NsType *ns,
            IdType id,
            const char *name)
            : SchemaResource<NsType,IdType>(ResourceType::kClass, ns, id, name) {};
    };

    template<class NsType, class IdType>
    class SchemaEnum : public SchemaResource<NsType,IdType> {

    public:
        constexpr SchemaEnum(
            const NsType *ns,
            IdType id,
            const char *name)
            : SchemaResource<NsType,IdType>(ResourceType::kEnum, ns, id, name) {};
    };

    enum class PropertyType {
        kNil,
        kBool,
        kInt64,
        kInt32,
        kInt16,
        kInt8,
        kUInt64,
        kUInt32,
        kUInt16,
        kUInt8,
        kFloat64,
        kString,
        kEnum,
        kProperty,
        kClass,
    };

    template<class NsType, class IdType>
    class SchemaProperty : public SchemaResource<NsType,IdType> {

    public:
        constexpr SchemaProperty(
            const NsType *ns,
            IdType id,
            const char *name,
            PropertyType propertyType)
            : SchemaResource<NsType,IdType>(ResourceType::kProperty, ns, id, name),
              m_propertyType(propertyType) {};

        constexpr PropertyType getPropertyType() const { return m_propertyType; }

    private:
        PropertyType m_propertyType;
    };

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