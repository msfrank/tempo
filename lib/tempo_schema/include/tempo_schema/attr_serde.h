#ifndef TEMPO_SCHEMA_ATTR_SERDE_H
#define TEMPO_SCHEMA_ATTR_SERDE_H

#include "abstract_attr_parser.h"
#include "abstract_attr_validator.h"
#include "abstract_attr_writer.h"
#include "attr.h"

namespace tempo_schema {

    class AttrValidator : public AbstractAttrValidator {
    public:
        explicit AttrValidator(const ComparableResource *resource);
        const ComparableResource *getResource() const override;
        AttrKey getKey() const override;
    private:
        const ComparableResource *m_resource;
    };

    /**
     *
     * @tparam T
     */
    template<class T>
    class AttrSerde : public AttrValidator {
    public:
        explicit AttrSerde(const ComparableResource *resource);
        virtual tempo_utils::Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const T &value) const = 0;
        virtual tempo_utils::Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, T &value) const = 0;
    };

    template<class T>
    AttrSerde<T>::AttrSerde(const ComparableResource *resource)
        : AttrValidator(resource)
    {
    }

    class NilAttr : public AttrSerde<std::nullptr_t> {

        using SerdeType = std::nullptr_t;

    public:
        explicit NilAttr(const ComparableResource *resource);
        tempo_utils::Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const std::nullptr_t &value) const override;
        tempo_utils::Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, std::nullptr_t &value) const override;
    };

    class BoolAttr : public AttrSerde<bool> {

        using SerdeType = bool;

    public:
        explicit BoolAttr(const ComparableResource *resource);
        tempo_utils::Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const bool &value) const override;
        tempo_utils::Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, bool &value) const override;
    };

    class Int64Attr : public AttrSerde<tu_int64> {

        using SerdeType = tu_int64;

    public:
        explicit Int64Attr(const ComparableResource *resource);
        tempo_utils::Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const tu_int64 &value) const override;
        tempo_utils::Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, tu_int64 &value) const override;
    };

    class UInt64Attr : public AttrSerde<uint64_t> {

        using SerdeType = uint64_t;

    public:
        explicit UInt64Attr(const ComparableResource *resource);
        tempo_utils::Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const uint64_t &value) const override;
        tempo_utils::Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, uint64_t &value) const override;
    };

    class UInt32Attr : public AttrSerde<tu_uint32> {

        using SerdeType = tu_uint32;

    public:
        explicit UInt32Attr(const ComparableResource *resource);
        tempo_utils::Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const tu_uint32 &value) const override;
        tempo_utils::Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, tu_uint32 &value) const override;
    };

    class UInt16Attr : public AttrSerde<tu_uint16> {

        using SerdeType = tu_uint16;

    public:
        explicit UInt16Attr(const ComparableResource *resource);
        tempo_utils::Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const tu_uint16 &value) const override;
        tempo_utils::Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, tu_uint16 &value) const override;
    };

    class UInt8Attr : public AttrSerde<tu_uint8> {

        using SerdeType = tu_uint8;

    public:
        explicit UInt8Attr(const ComparableResource *resource);
        tempo_utils::Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const tu_uint8 &value) const override;
        tempo_utils::Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, tu_uint8 &value) const override;
    };

    class DoubleAttr : public AttrSerde<double> {

        using SerdeType = double;

    public:
        explicit DoubleAttr(const ComparableResource *resource);
        tempo_utils::Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const double &value) const override;
        tempo_utils::Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, double &value) const override;
    };

    class StringAttr : public AttrSerde<std::string> {

        using SerdeType = std::string;

    public:
        explicit StringAttr(const ComparableResource *resource);
        tempo_utils::Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const std::string &value) const override;
        tempo_utils::Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, std::string &value) const override;
    };
}

#endif // TEMPO_SCHEMA_ATTR_SERDE_H
