#ifndef TEMPO_UTILS_ATTR_H
#define TEMPO_UTILS_ATTR_H

#include <string>

#include <absl/container/flat_hash_map.h>

#include "attr_result.h"
#include "integer_types.h"
#include "result.h"
#include "schema.h"
#include "url.h"

namespace tempo_utils {

    struct AttrKey {
        const char *ns;
        uint32_t id;
    };

    bool operator==(const AttrKey &lhs, const AttrKey &rhs);

    template <typename H>
    H AbslHashValue(H h, const AttrKey &attrKey) {
        return H::combine(std::move(h), attrKey.ns, attrKey.id);
    }

    struct AttrHandle {
        tu_uint32 handle;
    };

    enum class ValueType : tu_uint8 {
        Invalid,
        Nil,
        Bool,
        Int64,
        Float64,
        UInt64,
        UInt32,
        UInt16,
        UInt8,
        String,
        Handle,
    };

    class AttrValue {
    public:
        AttrValue();
        explicit AttrValue(std::nullptr_t nil);
        explicit AttrValue(bool b);
        explicit AttrValue(tu_int64 i64);
        explicit AttrValue(double f64);
        explicit AttrValue(tu_uint64 u64);
        explicit AttrValue(tu_uint32 u32);
        explicit AttrValue(tu_uint16 u16);
        explicit AttrValue(tu_uint8 u8);
        explicit AttrValue(const std::string &str);
        explicit AttrValue(std::string_view str);
        explicit AttrValue(const char *str);
        explicit AttrValue(const AttrHandle handle);
        AttrValue(const AttrValue &other);
        AttrValue(AttrValue &&other) noexcept;

        AttrValue& operator=(const AttrValue &other);
        AttrValue& operator=(AttrValue &&other) noexcept;

        bool isValid() const;
        ValueType getType() const;

        bool getBool() const;
        tu_int64 getInt64() const;
        double getFloat64() const;
        tu_uint64 getUInt64() const;
        tu_uint32 getUInt32() const;
        tu_uint16 getUInt16() const;
        tu_uint8 getUInt8() const;
        std::string getString() const;
        std::string_view stringView() const;
        AttrHandle getHandle() const;

    private:
        struct Priv {
            ValueType type;
            union {
                bool b;
                tu_int64 i64;
                double f64;
                tu_uint64 u64;
                tu_uint32 u32;
                tu_uint16 u16;
                tu_uint8 u8;
                const char *str;
                AttrHandle handle;
            } value;
            Priv();
            ~Priv();
        };
        std::shared_ptr<Priv> m_priv;
    };

    typedef std::pair<AttrKey,AttrValue> Attr;
    typedef absl::flat_hash_map<AttrKey,AttrValue> AttrMap;

    class AbstractAttrWriter {
    public:
        virtual ~AbstractAttrWriter() = default;
        virtual Result<tu_uint32> putNamespace(const tempo_utils::Url &nsUrl) = 0;
        virtual Result<tu_uint32> putNil() = 0;
        virtual Result<tu_uint32> putBool(bool b) = 0;
        virtual Result<tu_uint32> putInt64(tu_int64 i64) = 0;
        virtual Result<tu_uint32> putInt32(tu_int32 i32);           // if not overridden then putInt64 is used
        virtual Result<tu_uint32> putInt16(tu_int16 i16);           // if not overridden then putInt64 is used
        virtual Result<tu_uint32> putInt8(tu_int8 i8);              // if not overridden then putInt64 is used
        virtual Result<tu_uint32> putUInt64(tu_uint64 u64) = 0;
        virtual Result<tu_uint32> putUInt32(tu_uint32 u32);         // if not overridden then putUInt64 is used
        virtual Result<tu_uint32> putUInt16(tu_uint16 u16);         // if not overridden then putUInt64 is used
        virtual Result<tu_uint32> putUInt8(tu_uint8 u8);            // if not overridden then putUInt64 is used
        virtual Result<tu_uint32> putFloat64(double dbl) = 0;
        virtual Result<tu_uint32> putString(std::string_view str) = 0;
    };

    class AbstractAttrParser {
    public:
        virtual ~AbstractAttrParser() = default;
        virtual Status getNil(tu_uint32 index, std::nullptr_t &nil) = 0;
        virtual Status getBool(tu_uint32 index, bool &b) = 0;
        virtual Status getInt64(tu_uint32 index, tu_int64 &i64) = 0;
        virtual Status getInt32(tu_uint32 index, tu_int32 &i32);    // if not overridden then getInt64 is used
        virtual Status getInt16(tu_uint32 index, tu_int16 &i16);    // if not overridden then getInt64 is used
        virtual Status getInt8(tu_uint32 index, tu_int8 &i8);       // if not overridden then getInt64 is used
        virtual Status getUInt64(tu_uint32 index, tu_uint64 &u64) = 0;
        virtual Status getUInt32(tu_uint32 index, tu_uint32 &u32);  // if not overridden then getUInt64 is used
        virtual Status getUInt16(tu_uint32 index, tu_uint16 &u16);  // if not overridden then getUInt64 is used
        virtual Status getUInt8(tu_uint32 index, tu_uint8 &u8);     // if not overridden then getUInt64 is used
        virtual Status getFloat64(tu_uint32 index, double &dbl) = 0;
        virtual Status getString(tu_uint32 index, std::string &str) = 0;
    };

    /**
     * The AttrValidator interface.
     */
    class AbstractAttrValidator {
    public:
        virtual ~AbstractAttrValidator() = default;
        virtual const ComparableResource *getResource() const = 0;
        virtual AttrKey getKey() const = 0;
    };

    class AttrValidator : public AbstractAttrValidator {
    public:
        explicit AttrValidator(const ComparableResource *resource);
        virtual ~AttrValidator() = default;
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
        virtual Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const T &value) const = 0;
        virtual Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, T &value) const = 0;
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
        Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const std::nullptr_t &value) const override;
        Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, std::nullptr_t &value) const override;
    };

    class BoolAttr : public AttrSerde<bool> {

        using SerdeType = bool;

    public:
        explicit BoolAttr(const ComparableResource *resource);
        Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const bool &value) const override;
        Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, bool &value) const override;
    };

    class Int64Attr : public AttrSerde<tu_int64> {

        using SerdeType = tu_int64;

    public:
        explicit Int64Attr(const ComparableResource *resource);
        Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const tu_int64 &value) const override;
        Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, tu_int64 &value) const override;
    };

    class Int32Attr : public AttrSerde<tu_int32> {

        using SerdeType = tu_int32;

    public:
        explicit Int32Attr(const ComparableResource *resource);
        Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const tu_int32 &value) const override;
        Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, tu_int32 &value) const override;
    };

    class Int16Attr : public AttrSerde<tu_int16> {

        using SerdeType = tu_int16;

    public:
        explicit Int16Attr(const ComparableResource *resource);
        Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const tu_int16 &value) const override;
        Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, tu_int16 &value) const override;
    };

    class Int8Attr : public AttrSerde<tu_int8> {

        using SerdeType = tu_int8;

    public:
        explicit Int8Attr(const ComparableResource *resource);
        Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const tu_int8 &value) const override;
        Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, tu_int8 &value) const override;
    };

    class UInt64Attr : public AttrSerde<uint64_t> {

        using SerdeType = uint64_t;

    public:
        explicit UInt64Attr(const ComparableResource *resource);
        Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const uint64_t &value) const override;
        Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, uint64_t &value) const override;
    };

    class UInt32Attr : public AttrSerde<tu_uint32> {

        using SerdeType = tu_uint32;

    public:
        explicit UInt32Attr(const ComparableResource *resource);
        Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const tu_uint32 &value) const override;
        Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, tu_uint32 &value) const override;
    };

    class UInt16Attr : public AttrSerde<tu_uint16> {

        using SerdeType = tu_uint16;

    public:
        explicit UInt16Attr(const ComparableResource *resource);
        Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const tu_uint16 &value) const override;
        Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, tu_uint16 &value) const override;
    };

    class UInt8Attr : public AttrSerde<tu_uint8> {

        using SerdeType = tu_uint8;

    public:
        explicit UInt8Attr(const ComparableResource *resource);
        Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const tu_uint8 &value) const override;
        Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, tu_uint8 &value) const override;
    };

    class DoubleAttr : public AttrSerde<double> {

        using SerdeType = double;

    public:
        explicit DoubleAttr(const ComparableResource *resource);
        Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const double &value) const override;
        Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, double &value) const override;
    };

    class StringAttr : public AttrSerde<std::string> {

        using SerdeType = std::string;

    public:
        explicit StringAttr(const ComparableResource *resource);
        Result<tu_uint32> writeAttr(AbstractAttrWriter *writer, const std::string &value) const override;
        Status parseAttr(tu_uint32 index, AbstractAttrParser *parser, std::string &value) const override;
    };

    /**
     * @tparam StateType
     */
    template<class PStateType>
    class AbstractAttrParserWithState : public AbstractAttrParser {
    public:
        virtual ~AbstractAttrParserWithState() = default;
        virtual PStateType *getParserState() = 0;
        virtual tempo_utils::Status getHandle(tu_uint32 index, AttrHandle &handle) = 0;
    };

    /**
     * @tparam StateType
     */
    template<class WStateType>
    class AbstractAttrWriterWithState : public AbstractAttrWriter {
    public:
        virtual ~AbstractAttrWriterWithState() = default;
        virtual WStateType *getWriterState() = 0;
        virtual tempo_utils::Result<tu_uint32> putHandle(AttrHandle handle) = 0;
    };

    /**
     *
     */
    template<class ParseType, class StateType>
    class StatefulParsingSerde {
    public:
        virtual Status parseAttr(
            tu_uint32 index,
            AbstractAttrParserWithState<StateType> *parser,
            ParseType &value) const = 0;
    };

    /**
     *
     */
    template<class WriteType, class StateType>
    class StatefulWritingSerde {
    public:
        virtual Result<tu_uint32> writeAttr(
            AbstractAttrWriterWithState<StateType> *writer,
            const WriteType &value) const = 0;
    };
};

#endif // TEMPO_UTILS_ATTR_H