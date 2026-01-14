#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_schema/attr_serde.h>
#include <tempo_schema/schema_identifier.h>
#include <tempo_schema/schema_namespace.h>
#include <tempo_schema/schema_result.h>

#include "mock_attr_parser.h"
#include "mock_attr_writer.h"

static tempo_schema::SchemaNs ns("test");
static tempo_schema::SchemaId id(1);
static tempo_schema::SchemaProperty prop(&ns, id.getId(), "Prop", tempo_schema::PropertyType::kBool);

TEST(BoolAttr, WriteAttr) {
    tempo_schema::BoolAttr boolAttr(&prop);

    bool boolValue = true;
    tu_uint32 writeIndex = 0;

    MockAttrWriter writer;
    tempo_utils::Result<tu_uint32> result{writeIndex};

    EXPECT_CALL (writer, putBool(boolValue))
        .Times(testing::Exactly(1))
        .WillOnce(testing::Return(result));

    auto writeAttrResult = boolAttr.writeAttr(&writer, boolValue);
    ASSERT_TRUE (writeAttrResult.isResult());
    auto index = writeAttrResult.getResult();
    ASSERT_EQ (writeIndex, index);
}

TEST(BoolAttr, ParseAttr) {
    tempo_schema::BoolAttr boolAttr(&prop);

    MockAttrParser parser;

    bool boolValue = true;
    EXPECT_CALL (parser, getBool(testing::_, testing::_))
        .Times(testing::Exactly(1))
        .WillOnce(
            testing::DoAll(testing::SetArgReferee<1>(boolValue), testing::Return(tempo_utils::Status{})));

    bool result;
    auto parseAttrStatus = boolAttr.parseAttr(0, &parser, result);
    ASSERT_TRUE (parseAttrStatus.isOk());
    ASSERT_EQ (boolValue, result);
}

TEST(BoolAttr, ParseFailsWhenWrongType) {
    tempo_schema::BoolAttr boolAttr(&prop);

    MockAttrParser parser;
    auto status = tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kWrongType);

    EXPECT_CALL (parser, getBool(testing::_, testing::_))
        .Times(testing::Exactly(1))
        .WillOnce(testing::Return(status));

    bool boolValue;
    auto parseAttrStatus = boolAttr.parseAttr(0, &parser, boolValue);
    ASSERT_TRUE (parseAttrStatus.matchesCondition(tempo_schema::SchemaCondition::kWrongType));
}
