#ifndef TEMPO_SCHEMA_PATH_SERDE_H
#define TEMPO_SCHEMA_PATH_SERDE_H

#include <filesystem>

#include "attr_serde.h"

namespace tempo_schema {

    /**
     * Path attr serializer/deserializer.
     */
    class PathAttr : public AttrSerde<std::filesystem::path> {

        using SerdeType = std::filesystem::path;

    public:
        explicit PathAttr(const ComparableResource *resource);
        tempo_utils::Result<tu_uint32> writeAttr(
            AbstractAttrWriter *writer,
            const std::filesystem::path &value) const override;
        tempo_utils::Status parseAttr(
            tu_uint32 index,
            AbstractAttrParser *parser,
            std::filesystem::path &value) const override;
    };
}

#endif // TEMPO_SCHEMA_PATH_SERDE_H