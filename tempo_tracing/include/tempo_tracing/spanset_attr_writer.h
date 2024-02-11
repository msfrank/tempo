#ifndef TEMPO_TRACING_SPANSET_ATTR_WRITER_H
#define TEMPO_TRACING_SPANSET_ATTR_WRITER_H

#include <tempo_utils/attr.h>

namespace tempo_tracing {

    class SpansetAttrWriter : public tempo_utils::AbstractAttrWriter {

    public:
        SpansetAttrWriter();
        tempo_utils::Result<tu_uint32> putNamespace(const tempo_utils::Url &nsUrl) override;
        tempo_utils::Result<tu_uint32> putNil() override;
        tempo_utils::Result<tu_uint32> putBool(bool b) override;
        tempo_utils::Result<tu_uint32> putInt64(tu_int64 i64) override;
        tempo_utils::Result<tu_uint32> putUInt64(tu_uint64 u64) override;
        tempo_utils::Result<tu_uint32> putFloat64(double dbl) override;
        tempo_utils::Result<tu_uint32> putString(std::string_view str) override;
        tempo_utils::AttrValue getValue() const;

    private:
        tempo_utils::AttrValue m_value;
    };
}

#endif // TEMPO_TRACING_SPANSET_ATTR_WRITER_H