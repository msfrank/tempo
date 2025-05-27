#include <absl/hash/hash_testing.h>

#include <tempo_config/config_types.h>
#include <tempo_config/parse_config.h>

TEST(ConfigCompare, CompareNil)
{
    ASSERT_EQ (tempo_config::ConfigNil().toNode(), tempo_config::ConfigNil().toNode());
    ASSERT_EQ (tempo_config::ConfigNode().toNode(), tempo_config::ConfigNil().toNode());
    ASSERT_NE (tempo_config::ConfigValue().toNode(), tempo_config::ConfigNil().toNode());
    ASSERT_NE (tempo_config::ConfigSeq().toNode(), tempo_config::ConfigNil().toNode());
    ASSERT_NE (tempo_config::ConfigMap().toNode(), tempo_config::ConfigNil().toNode());
}

TEST(ConfigCompare, CompareValue)
{
    ASSERT_EQ (tempo_config::ConfigValue(), tempo_config::ConfigValue());
    ASSERT_EQ (tempo_config::ConfigValue("value"), tempo_config::ConfigValue("value"));
    ASSERT_NE (tempo_config::ConfigValue("a"), tempo_config::ConfigValue("b"));
    ASSERT_LT (tempo_config::ConfigValue("a"), tempo_config::ConfigValue("b"));
    ASSERT_LT (tempo_config::ConfigValue("a"), tempo_config::ConfigValue("aa"));
    ASSERT_LT (tempo_config::ConfigValue("a"), tempo_config::ConfigValue("bb"));
    ASSERT_LT (tempo_config::ConfigValue("ab"), tempo_config::ConfigValue("b"));
    ASSERT_LT (tempo_config::ConfigValue("bbbba"), tempo_config::ConfigValue("bbbbb"));
}

TEST(ConfigCompare, CompareSeq) {
    ASSERT_EQ (tempo_config::ConfigSeq(), tempo_config::ConfigSeq());
    ASSERT_EQ (
        tempo_config::ConfigSeq({tempo_config::ConfigValue("a")}),
        tempo_config::ConfigSeq({tempo_config::ConfigValue("a")})
        );
    ASSERT_NE (
        tempo_config::ConfigSeq({tempo_config::ConfigValue("a")}),
        tempo_config::ConfigSeq({tempo_config::ConfigValue("b")})
        );
    ASSERT_LT (
        tempo_config::ConfigSeq({tempo_config::ConfigValue("a")}),
        tempo_config::ConfigSeq({tempo_config::ConfigValue("b")})
        );
    ASSERT_LT (
        tempo_config::ConfigSeq({tempo_config::ConfigValue("a")}),
        tempo_config::ConfigSeq({
            tempo_config::ConfigValue("a"),
            tempo_config::ConfigValue("a"),
            })
        );
    ASSERT_LT (
        tempo_config::ConfigSeq({tempo_config::ConfigValue("a")}),
        tempo_config::ConfigSeq({
            tempo_config::ConfigValue("b"),
            tempo_config::ConfigValue("b"),
            })
        );
    ASSERT_LT (
        tempo_config::ConfigSeq({
            tempo_config::ConfigValue("a"),
            tempo_config::ConfigValue("b"),
            }),
        tempo_config::ConfigSeq({tempo_config::ConfigValue("b")})
        );
    ASSERT_LT (
        tempo_config::ConfigSeq({
            tempo_config::ConfigValue("b"),
            tempo_config::ConfigValue("b"),
            tempo_config::ConfigValue("b"),
            tempo_config::ConfigValue("b"),
            tempo_config::ConfigValue("a"),
            }),
        tempo_config::ConfigSeq({
            tempo_config::ConfigValue("b"),
            tempo_config::ConfigValue("b"),
            tempo_config::ConfigValue("b"),
            tempo_config::ConfigValue("b"),
            tempo_config::ConfigValue("b"),
            })
        );
}

TEST(ConfigCompare, CompareMap)
{
    ASSERT_EQ (tempo_config::ConfigMap(), tempo_config::ConfigMap());
    ASSERT_EQ (
        tempo_config::ConfigMap({{"one",tempo_config::ConfigValue("1")}}),
        tempo_config::ConfigMap({{"one",tempo_config::ConfigValue("1")}})
        );
    ASSERT_NE (
        tempo_config::ConfigMap({{"one",tempo_config::ConfigValue("1")}}),
        tempo_config::ConfigMap({{"two",tempo_config::ConfigValue("2")}})
        );
    ASSERT_LT (
        tempo_config::ConfigMap({{"one",tempo_config::ConfigValue("1")}}),
        tempo_config::ConfigMap({
            {"one",tempo_config::ConfigValue("1")},
            {"two",tempo_config::ConfigValue("2")},
            })
        );
}
