#ifndef TEMPO_SCHEMA_SCHEMA_IDENTIFIER_H
#define TEMPO_SCHEMA_SCHEMA_IDENTIFIER_H

#include <tempo_utils/integer_types.h>

namespace tempo_schema {

    class SchemaId {

    public:
        constexpr explicit SchemaId(tu_uint32 id) : m_id(id) {};
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
}

#endif // TEMPO_SCHEMA_SCHEMA_IDENTIFIER_H
