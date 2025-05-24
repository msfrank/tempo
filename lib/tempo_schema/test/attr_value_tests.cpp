#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_schema/attr.h>

TEST(AttrValue, ConstructNil) {
    tempo_schema::AttrValue nilValue(nullptr);
    ASSERT_EQ (tempo_schema::ValueType::Nil, nilValue.getType());
}

TEST(AttrValue, ConstructTrue) {
    tempo_schema::AttrValue trueValue(true);
    ASSERT_EQ (tempo_schema::ValueType::Bool, trueValue.getType());
    ASSERT_EQ (true, trueValue.getBool());
}

TEST(AttrValue, ConstructFalse) {
    tempo_schema::AttrValue falseValue(false);
    ASSERT_EQ (tempo_schema::ValueType::Bool, falseValue.getType());
    ASSERT_EQ (false, falseValue.getBool());
}
