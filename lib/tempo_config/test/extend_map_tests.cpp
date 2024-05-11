#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <tempo_config/extend_map.h>

TEST(ExtendMap, TestExtendBaseWithEmptyOverride)
{
    tempo_config::ConfigMap baseMap{
        {"foo", tempo_config::ConfigValue("1")},
    };
    tempo_config::ConfigMap overrideMap{};

    auto extendMapResult = tempo_config::extend_map(baseMap, overrideMap);
    ASSERT_TRUE (extendMapResult.isResult());
    auto mergedMap = extendMapResult.getResult();
    ASSERT_EQ (baseMap, mergedMap);
}

TEST(ExtendMap, TestExtendEmptyBaseWithOverride)
{
    tempo_config::ConfigMap baseMap{};
    tempo_config::ConfigMap overrideMap{
        {"foo", tempo_config::ConfigValue("1")},
    };

    auto extendMapResult = tempo_config::extend_map(baseMap, overrideMap);
    ASSERT_TRUE (extendMapResult.isResult());
    auto mergedMap = extendMapResult.getResult();
    ASSERT_EQ (overrideMap, mergedMap);
}

TEST(ExtendMap, TestExtendBaseWithOverrideNoConflicts)
{
    tempo_config::ConfigMap baseMap{
        {"foo", tempo_config::ConfigValue("1")},
    };
    tempo_config::ConfigMap overrideMap{
        {"bar", tempo_config::ConfigValue("2")},
    };

    auto extendMapResult = tempo_config::extend_map(baseMap, overrideMap);
    ASSERT_TRUE (extendMapResult.isResult());
    auto mergedMap = extendMapResult.getResult();

    ASSERT_EQ ((tempo_config::ConfigMap{
        {"foo", tempo_config::ConfigValue("1")},
        {"bar", tempo_config::ConfigValue("2")},
    }), mergedMap);
}

TEST(ExtendMap, TestExtendBaseDescendentNoConflicts)
{
    tempo_config::ConfigMap baseMap{
        {"foo", tempo_config::ConfigMap{
            {"bar", tempo_config::ConfigMap()},
        }},
        {"baz", tempo_config::ConfigValue("2")},
    };
    tempo_config::ConfigMap overrideMap{
        {"foo", tempo_config::ConfigMap{
            {"bar", tempo_config::ConfigMap{
                {"qux", tempo_config::ConfigValue("1")},
            }},
        }},
    };

    auto extendMapResult = tempo_config::extend_map(baseMap, overrideMap);
    ASSERT_TRUE (extendMapResult.isResult());
    auto mergedMap = extendMapResult.getResult();

    ASSERT_EQ ((tempo_config::ConfigMap{
        {"foo", tempo_config::ConfigMap{
            {"bar", tempo_config::ConfigMap{
                {"qux", tempo_config::ConfigValue("1")},
            }},
        }},
        {"baz", tempo_config::ConfigValue("2")},
    }), mergedMap);
}

TEST(ExtendMap, TestExtendBaseEntryIsOverriddenWithSameType)
{
    tempo_config::ConfigMap baseMap{
        {"foo", tempo_config::ConfigValue("1")},
    };
    tempo_config::ConfigMap overrideMap{
        {"foo", tempo_config::ConfigValue("2")},
    };

    auto extendMapResult = tempo_config::extend_map(baseMap, overrideMap);
    ASSERT_TRUE (extendMapResult.isStatus());
}

TEST(ExtendMap, TestExtendBaseDescendentIsOverriddenWithSameType)
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

    auto extendMapResult = tempo_config::extend_map(baseMap, overrideMap);
    ASSERT_TRUE (extendMapResult.isStatus());
}

TEST(ExtendMap, TestExtendBaseDescendentIsOverriddenWithDifferentType)
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

    auto extendMapResult = tempo_config::extend_map(baseMap, overrideMap);
    ASSERT_TRUE (extendMapResult.isStatus());
}
