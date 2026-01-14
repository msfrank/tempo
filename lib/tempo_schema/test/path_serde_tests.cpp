#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_schema/path_serde.h>
#include <tempo_schema/schema_identifier.h>
#include <tempo_schema/schema_namespace.h>
#include <tempo_schema/schema_result.h>

#include "mock_attr_parser.h"
#include "mock_attr_writer.h"

static tempo_schema::SchemaNs ns("test");
static tempo_schema::SchemaId id(1);
static tempo_schema::SchemaProperty prop(&ns, id.getId(), "Prop", tempo_schema::PropertyType::kString);

TEST(PathAttr, WriteAttr)
{
    tempo_schema::PathAttr pathAttr(&prop);

    std::filesystem::path path = std::filesystem::current_path();
    std::string pathValue = path.string();
    tu_uint32 writeIndex = 0;

    MockAttrWriter writer;
    tempo_utils::Result<tu_uint32> result{writeIndex};

    EXPECT_CALL (writer, putString(pathValue))
        .Times(testing::Exactly(1))
        .WillOnce(testing::Return(result));

    auto writeAttrResult = pathAttr.writeAttr(&writer, pathValue);
    ASSERT_TRUE (writeAttrResult.isResult());
    auto index = writeAttrResult.getResult();
    ASSERT_EQ (writeIndex, index);
}

TEST(PathAttr, ParseAttr)
{
    tempo_schema::PathAttr pathAttr(&prop);

    MockAttrParser parser;

    std::filesystem::path path = std::filesystem::current_path();
    std::string pathValue = path.string();

    EXPECT_CALL (parser, getString(testing::_, testing::_))
        .Times(testing::Exactly(1))
        .WillOnce(
            testing::DoAll(testing::SetArgReferee<1>(pathValue), testing::Return(tempo_utils::Status{})));

    std::filesystem::path result;
    auto parseAttrStatus = pathAttr.parseAttr(0, &parser, result);
    ASSERT_TRUE (parseAttrStatus.isOk());
    ASSERT_EQ (path, result);
}

TEST(PathAttr, ParseFailsWhenWrongType)
{
    tempo_schema::PathAttr pathAttr(&prop);

    MockAttrParser parser;
    auto status = tempo_schema::SchemaStatus::forCondition(tempo_schema::SchemaCondition::kWrongType);

    EXPECT_CALL (parser, getString(testing::_, testing::_))
        .Times(testing::Exactly(1))
        .WillOnce(testing::Return(status));

    std::filesystem::path result;
    auto parseAttrStatus = pathAttr.parseAttr(0, &parser, result);
    ASSERT_TRUE (parseAttrStatus.matchesCondition(tempo_schema::SchemaCondition::kWrongType));
}
