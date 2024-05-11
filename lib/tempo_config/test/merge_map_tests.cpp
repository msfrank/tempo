#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <tempo_config/merge_map.h>

TEST(MergeMap, TestMergeBaseWithEmptyOverride)
{
    tempo_config::ConfigMap baseMap{
        {"foo", tempo_config::ConfigValue("1")},
    };
    tempo_config::ConfigMap overrideMap{};

    auto mergedMap = tempo_config::merge_map(baseMap, overrideMap);
    ASSERT_EQ (baseMap, mergedMap);
}

TEST(MergeMap, TestMergeEmptyBaseWithOverride)
{
    tempo_config::ConfigMap baseMap{};
    tempo_config::ConfigMap overrideMap{
        {"foo", tempo_config::ConfigValue("1")},
    };

    auto mergedMap = tempo_config::merge_map(baseMap, overrideMap);
    ASSERT_EQ (overrideMap, mergedMap);
}

TEST(MergeMap, TestMergeBaseWithOverrideNoConflicts)
{
    tempo_config::ConfigMap baseMap{
        {"foo", tempo_config::ConfigValue("1")},
    };
    tempo_config::ConfigMap overrideMap{
        {"bar", tempo_config::ConfigValue("2")},
    };

    auto mergedMap = tempo_config::merge_map(baseMap, overrideMap);

    ASSERT_EQ ((tempo_config::ConfigMap{
        {"foo", tempo_config::ConfigValue("1")},
        {"bar", tempo_config::ConfigValue("2")},
    }), mergedMap);
}

TEST(MergeMap, TestMergeBaseEntryIsOverriddenWithSameType)
{
    tempo_config::ConfigMap baseMap{
        {"foo", tempo_config::ConfigValue("1")},
    };
    tempo_config::ConfigMap overrideMap{
        {"foo", tempo_config::ConfigValue("2")},
    };

    auto mergedMap = tempo_config::merge_map(baseMap, overrideMap);

    ASSERT_EQ ((tempo_config::ConfigMap{
        {"foo", tempo_config::ConfigValue("2")},
    }), mergedMap);
}

TEST(MergeMap, TestMergeBaseDescendentIsOverriddenWithSameType)
{
    tempo_config::ConfigMap baseMap{
        {"foo", tempo_config::ConfigMap{
            {"bar", tempo_config::ConfigValue("1")},
        }},
        {"baz", tempo_config::ConfigValue("2")},
    };
    tempo_config::ConfigMap overrideMap{
        {"foo", tempo_config::ConfigMap{
            {"bar", tempo_config::ConfigValue("3")},
        }},
    };

    auto mergedMap = tempo_config::merge_map(baseMap, overrideMap);

    ASSERT_EQ ((tempo_config::ConfigMap{
        {"foo", tempo_config::ConfigMap{
            {"bar", tempo_config::ConfigValue("3")},
        }},
        {"baz", tempo_config::ConfigValue("2")},
    }), mergedMap);
}

TEST(MergeMap, TestMergeBaseDescendentIsOverriddenWithDifferentType)
{
    tempo_config::ConfigMap baseMap{
        {"foo", tempo_config::ConfigMap{
            {"bar", tempo_config::ConfigValue("1")},
        }},
        {"baz", tempo_config::ConfigValue("2")},
    };
    tempo_config::ConfigMap overrideMap{
        {"foo", tempo_config::ConfigMap{
            {"bar", tempo_config::ConfigNil()},
        }},
    };

    auto mergedMap = tempo_config::merge_map(baseMap, overrideMap);

    ASSERT_EQ ((tempo_config::ConfigMap{
        {"foo", tempo_config::ConfigMap{
            {"bar", tempo_config::ConfigNil()},
        }},
        {"baz", tempo_config::ConfigValue("2")},
    }), mergedMap);
}
