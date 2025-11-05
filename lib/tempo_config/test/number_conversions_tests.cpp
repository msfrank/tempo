#include "tempo_config/config_builder.h"
#include "tempo_test/tempo_test.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_config/number_conversions.h>

TEST(NumberConversions, TestConvertInt64)
{
    tu_int64 i64;

    tempo_config::Int64Parser parser;
    ASSERT_THAT (parser.convertValue(tempo_config::valueNode("42"), i64), tempo_test::IsOk());
    ASSERT_EQ (42, i64);

    tempo_config::Int64Parser parserWithDefault(42);
    ASSERT_THAT (parserWithDefault.convertValue(tempo_config::nilNode(), i64), tempo_test::IsOk());
    ASSERT_EQ (42, i64);
}
