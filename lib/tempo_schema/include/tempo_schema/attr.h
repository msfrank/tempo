#ifndef TEMPO_UTILS_ATTR_H
#define TEMPO_UTILS_ATTR_H

#include <string>

#include <absl/container/flat_hash_map.h>

#include <tempo_utils/integer_types.h>
#include <tempo_utils/result.h>
#include <tempo_utils/url.h>

namespace tempo_schema {

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
}

#endif // TEMPO_UTILS_ATTR_H