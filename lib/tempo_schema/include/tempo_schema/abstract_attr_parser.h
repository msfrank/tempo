#ifndef TEMPO_SCHEMA_ABSTRACT_ATTR_PARSER_H
#define TEMPO_SCHEMA_ABSTRACT_ATTR_PARSER_H

#include <tempo_utils/status.h>

namespace tempo_schema {

    class AbstractAttrParser {
    public:
        virtual ~AbstractAttrParser() = default;

        virtual tempo_utils::Status getNil(tu_uint32 index, std::nullptr_t &nil) = 0;
        virtual tempo_utils::Status getBool(tu_uint32 index, bool &b) = 0;
        virtual tempo_utils::Status getInt64(tu_uint32 index, tu_int64 &i64) = 0;
        virtual tempo_utils::Status getUInt64(tu_uint32 index, tu_uint64 &u64) = 0;
        virtual tempo_utils::Status getUInt32(tu_uint32 index, tu_uint32 &u32) = 0;
        virtual tempo_utils::Status getUInt16(tu_uint32 index, tu_uint16 &u16) = 0;
        virtual tempo_utils::Status getUInt8(tu_uint32 index, tu_uint8 &u8) = 0;
        virtual tempo_utils::Status getFloat64(tu_uint32 index, double &dbl) = 0;
        virtual tempo_utils::Status getString(tu_uint32 index, std::string &str) = 0;
    };
}

#endif // TEMPO_SCHEMA_ABSTRACT_ATTR_PARSER_H
