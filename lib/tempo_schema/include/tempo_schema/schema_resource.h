#ifndef TEMPO_SCHEMA_SCHEMA_RESOURCE_H
#define TEMPO_SCHEMA_SCHEMA_RESOURCE_H

#include <string_view>

#include <tempo_utils/integer_types.h>

namespace tempo_schema {

    enum class ResourceType {
        kClass,
        kProperty,
        kEnum,
        kLiteral,
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

        template <typename H>
        friend H AbslHashValue(H h, const ComparableResource &resource) {
            return H::combine(std::move(h), std::string_view{resource.m_uri}, resource.m_id);
        }

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
}

#endif // TEMPO_SCHEMA_SCHEMA_RESOURCE_H
