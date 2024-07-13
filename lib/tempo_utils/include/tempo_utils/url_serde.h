#ifndef TEMPO_UTILS_URL_SERDE_H
#define TEMPO_UTILS_URL_SERDE_H

#include "attr.h"
#include "url.h"

namespace tempo_utils {

    /**
     * Url attr serializer/deserializer.
     */
    class UrlAttr : public AttrSerde<Url> {

        using SerdeType = Url;

    public:
        explicit UrlAttr(const ComparableResource *resource);
        Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const Url &value) const override;
        Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, Url &value) const override;
    };
}

#endif // TEMPO_UTILS_URL_SERDE_H
