#ifndef TEMPO_SCHEMA_ABSTRACT_ATTR_WRITER_H
#define TEMPO_SCHEMA_ABSTRACT_ATTR_WRITER_H

#include <tempo_utils/result.h>
#include <tempo_utils/url.h>

namespace tempo_schema {

    class AbstractAttrWriter {
    public:
        virtual ~AbstractAttrWriter() = default;

        virtual tempo_utils::Result<tu_uint32> putNamespace(const tempo_utils::Url &nsUrl) = 0;
        virtual tempo_utils::Result<tu_uint32> putNil() = 0;
        virtual tempo_utils::Result<tu_uint32> putBool(bool b) = 0;
        virtual tempo_utils::Result<tu_uint32> putInt64(tu_int64 i64) = 0;
        virtual tempo_utils::Result<tu_uint32> putUInt64(tu_uint64 u64) = 0;
        virtual tempo_utils::Result<tu_uint32> putUInt32(tu_uint32 u32) = 0;
        virtual tempo_utils::Result<tu_uint32> putUInt16(tu_uint16 u16) = 0;
        virtual tempo_utils::Result<tu_uint32> putUInt8(tu_uint8 u8) = 0;
        virtual tempo_utils::Result<tu_uint32> putFloat64(double dbl) = 0;
        virtual tempo_utils::Result<tu_uint32> putString(std::string_view str) = 0;
    };
}

#endif // TEMPO_SCHEMA_ABSTRACT_ATTR_WRITER_H
