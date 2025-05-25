#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_config/config_builder.h>

TEST(ConfigBuilder, TestBuildNil)
{
    auto node = tempo_config::nilNode();
    ASSERT_EQ (tempo_config::ConfigNodeType::kNil, node.getNodeType());
}

TEST(ConfigBuilder, TestBuildValue)
{
    auto node = tempo_config::valueNode("value");
    ASSERT_EQ (tempo_config::ConfigNodeType::kValue, node.getNodeType());
    ASSERT_EQ ("value", node.toValue().getValue());
}

TEST(ConfigBuilder, TestBuildSeq)
{
    auto node = tempo_config::startSeq()
        .append(tempo_config::ConfigValue("1"))
        .append(tempo_config::ConfigValue("2"))
        .append(tempo_config::ConfigValue("3"))
        .buildNode();

    ASSERT_EQ (tempo_config::ConfigNodeType::kSeq, node.getNodeType());
    auto seq = node.toSeq();
    ASSERT_EQ (3, seq.seqSize());
    auto e1 = seq.seqAt(0);
    ASSERT_EQ (tempo_config::ConfigNodeType::kValue, e1.getNodeType());
    ASSERT_EQ ("1", e1.toValue().getValue());
    auto e2 = seq.seqAt(1);
    ASSERT_EQ (tempo_config::ConfigNodeType::kValue, e2.getNodeType());
    ASSERT_EQ ("2", e2.toValue().getValue());
    auto e3 = seq.seqAt(2);
    ASSERT_EQ (tempo_config::ConfigNodeType::kValue, e3.getNodeType());
    ASSERT_EQ ("3", e3.toValue().getValue());
}

TEST(ConfigBuilder, TestBuildMap)
{
    auto node = tempo_config::startMap()
        .put("1", tempo_config::ConfigValue("1"))
        .put("2", tempo_config::ConfigValue("2"))
        .put("3", tempo_config::ConfigValue("3"))
        .buildNode();

    ASSERT_EQ (tempo_config::ConfigNodeType::kMap, node.getNodeType());
    auto map = node.toMap();
    ASSERT_EQ (3, map.mapSize());
    auto e1 = map.mapAt("1");
    ASSERT_EQ (tempo_config::ConfigNodeType::kValue, e1.getNodeType());
    ASSERT_EQ ("1", e1.toValue().getValue());
    auto e2 = map.mapAt("2");
    ASSERT_EQ (tempo_config::ConfigNodeType::kValue, e2.getNodeType());
    ASSERT_EQ ("2", e2.toValue().getValue());
    auto e3 = map.mapAt("3");
    ASSERT_EQ (tempo_config::ConfigNodeType::kValue, e3.getNodeType());
    ASSERT_EQ ("3", e3.toValue().getValue());
}
