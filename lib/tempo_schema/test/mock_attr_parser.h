#ifndef MOCK_ATTR_PARSER_H
#define MOCK_ATTR_PARSER_H

#include <gmock/gmock.h>

#include <tempo_schema/abstract_attr_parser.h>

class MockAttrParser : public tempo_schema::AbstractAttrParser {
public:
    MOCK_METHOD(tempo_utils::Status, getNil, (tu_uint32 index, std::nullptr_t &nil), (override));
    MOCK_METHOD(tempo_utils::Status, getBool, (tu_uint32 index, bool &b), (override));
    MOCK_METHOD(tempo_utils::Status, getInt64, (tu_uint32 index, tu_int64 &i64), (override));
    MOCK_METHOD(tempo_utils::Status, getUInt64, (tu_uint32 index, tu_uint64 &u64), (override));
    MOCK_METHOD(tempo_utils::Status, getUInt32, (tu_uint32 index, tu_uint32 &u32), (override));
    MOCK_METHOD(tempo_utils::Status, getUInt16, (tu_uint32 index, tu_uint16 &u16), (override));
    MOCK_METHOD(tempo_utils::Status, getUInt8, (tu_uint32 index, tu_uint8 &u8), (override));
    MOCK_METHOD(tempo_utils::Status, getFloat64, (tu_uint32 index, double &dbl), (override));
    MOCK_METHOD(tempo_utils::Status, getString, (tu_uint32 index, std::string &str), (override));
};

#endif // MOCK_ATTR_PARSER_H
