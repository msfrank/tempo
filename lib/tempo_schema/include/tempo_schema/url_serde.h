#ifndef TEMPO_SCHEMA_URL_SERDE_H
#define TEMPO_SCHEMA_URL_SERDE_H

#include <tempo_utils/url.h>

#include "attr_serde.h"

namespace tempo_schema {

    /**
     * Url attr serializer/deserializer.
     */
    class UrlAttr : public AttrSerde<tempo_utils::Url> {

        using SerdeType = tempo_utils::Url;

    public:
        explicit UrlAttr(const ComparableResource *resource);
        tempo_utils::Result<tu_uint32> writeAttr(
            AbstractAttrWriter *writer,
            const tempo_utils::Url &value) const override;
        tempo_utils::Status parseAttr(
            tu_uint32 index,
            AbstractAttrParser *parser,
            tempo_utils::Url &value) const override;
    };
}

#endif // TEMPO_SCHEMA_URL_SERDE_H
