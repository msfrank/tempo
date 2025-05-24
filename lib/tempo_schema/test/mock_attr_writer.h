#ifndef MOCK_ATTR_WRITER_H
#define MOCK_ATTR_WRITER_H

#include <gmock/gmock.h>

#include <tempo_schema/abstract_attr_writer.h>

class MockAttrWriter : public tempo_schema::AbstractAttrWriter {
public:
    MOCK_METHOD((tempo_utils::Result<tu_uint32>), putNamespace, (const tempo_utils::Url &nsUrl), (override));
    MOCK_METHOD((tempo_utils::Result<tu_uint32>), putNil, (), (override));
    MOCK_METHOD((tempo_utils::Result<tu_uint32>), putBool, (bool b), (override));
    MOCK_METHOD((tempo_utils::Result<tu_uint32>), putInt64, (tu_int64 i64), (override));
    MOCK_METHOD((tempo_utils::Result<tu_uint32>), putUInt64, (tu_uint64 u64), (override));
    MOCK_METHOD((tempo_utils::Result<tu_uint32>), putUInt32, (tu_uint32 u32), (override));
    MOCK_METHOD((tempo_utils::Result<tu_uint32>), putUInt16, (tu_uint16 u16), (override));
    MOCK_METHOD((tempo_utils::Result<tu_uint32>), putUInt8, (tu_uint8 u8), (override));
    MOCK_METHOD((tempo_utils::Result<tu_uint32>), putFloat64, (double dbl), (override));
    MOCK_METHOD((tempo_utils::Result<tu_uint32>), putString, (std::string_view str), (override));
};

#endif // MOCK_ATTR_WRITER_H
