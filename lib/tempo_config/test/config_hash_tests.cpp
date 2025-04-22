#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <absl/hash/hash_testing.h>

#include <tempo_config/config_types.h>
#include <tempo_config/parse_config.h>

TEST(ConfigHashing, ValueSupportsAbslHash)
{
    EXPECT_TRUE (absl::VerifyTypeImplementsAbslHashCorrectly({
        tempo_config::ConfigValue{},
        tempo_config::ConfigValue{"true"},
        tempo_config::ConfigValue{"false"},
        tempo_config::ConfigValue{"42"},
        tempo_config::ConfigValue{"3.14"},
        tempo_config::ConfigValue{"6.02e23"},
    }));
}

TEST(ConfigHashing, SeqSupportsAbslHash)
{
    EXPECT_TRUE (absl::VerifyTypeImplementsAbslHashCorrectly({
        tempo_config::ConfigSeq{},
        tempo_config::ConfigSeq{tempo_config::ConfigValue{"1"}},
        tempo_config::ConfigSeq{tempo_config::ConfigValue{"2"}},
        tempo_config::ConfigSeq{{
            tempo_config::ConfigValue{"1"},
            tempo_config::ConfigValue{"2"}}},
        tempo_config::ConfigSeq{{
            tempo_config::ConfigValue{"2"},
            tempo_config::ConfigValue{"3"}}},
        tempo_config::ConfigSeq{{
            tempo_config::ConfigValue{"1"},
            tempo_config::ConfigValue{"2"},
            tempo_config::ConfigValue{"3"}}},
        tempo_config::ConfigSeq{{tempo_config::ConfigNil{}}},
        tempo_config::ConfigSeq{{tempo_config::ConfigSeq{}}},
        tempo_config::ConfigSeq{{tempo_config::ConfigMap{}}},
    }));
}

TEST(ConfigHashing, MapSupportsAbslHash)
{
    EXPECT_TRUE (absl::VerifyTypeImplementsAbslHashCorrectly({
        tempo_config::ConfigMap{},
        tempo_config::ConfigMap{{"one", tempo_config::ConfigValue{"1"}}},
        tempo_config::ConfigMap{{"two", tempo_config::ConfigValue{"2"}}},
        tempo_config::ConfigMap{{
            {"one", tempo_config::ConfigValue{"1"}},
            {"two", tempo_config::ConfigValue{"2"}}}},
        tempo_config::ConfigMap{{
                {"two", tempo_config::ConfigValue{"2"}},
                {"three", tempo_config::ConfigValue{"3"}}}},
        tempo_config::ConfigMap{{
            {"one", tempo_config::ConfigValue{"1"}},
            {"two", tempo_config::ConfigValue{"2"}},
            {"three", tempo_config::ConfigValue{"3"}}}},
        tempo_config::ConfigMap{{{"nil", tempo_config::ConfigNil{}}}},
        tempo_config::ConfigMap{{{"seq", tempo_config::ConfigSeq{}}}},
        tempo_config::ConfigMap{{{"map", tempo_config::ConfigMap{}}}},
    }));
}