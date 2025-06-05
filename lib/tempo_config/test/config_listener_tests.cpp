
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <tempo_config/config_utils.h>

TEST(ConfigListener, ParseNull)
{
    auto readNilResult = tempo_config::read_config_string("null");
    ASSERT_TRUE (readNilResult.isResult());
    ASSERT_EQ (readNilResult.getResult(), tempo_config::ConfigNil());
}

TEST(ConfigListener, ParseTrueValue)
{
    auto readTrueResult = tempo_config::read_config_string("true");
    ASSERT_TRUE (readTrueResult.isResult());
    ASSERT_EQ (readTrueResult.getResult(), tempo_config::ConfigValue("true"));
}

TEST(ConfigListener, ParseFalseValue)
{
    auto readFalseResult = tempo_config::read_config_string("false");
    ASSERT_TRUE (readFalseResult.isResult());
    ASSERT_EQ (readFalseResult.getResult(), tempo_config::ConfigValue("false"));
}

TEST(ConfigListener, ParseIntegerValue)
{
    auto readNumberResult = tempo_config::read_config_string("42");
    ASSERT_TRUE (readNumberResult.isResult());
    ASSERT_EQ (readNumberResult.getResult(), tempo_config::ConfigValue("42"));
}

TEST(ConfigListener, ParseFixedFloatValue)
{
    auto readNumberResult = tempo_config::read_config_string("3.14159");
    ASSERT_TRUE (readNumberResult.isResult());
    ASSERT_EQ (readNumberResult.getResult(), tempo_config::ConfigValue("3.14159"));
}

TEST(ConfigListener, ParseScientificFloatValue)
{
    auto readNumberResult = tempo_config::read_config_string("6.022e23");
    ASSERT_TRUE (readNumberResult.isResult());
    ASSERT_EQ (readNumberResult.getResult(), tempo_config::ConfigValue("6.022e23"));
}

TEST(ConfigListener, ParseStringValue)
{
    auto readStringResult = tempo_config::read_config_string("\"hello, world!\"");
    ASSERT_TRUE (readStringResult.isResult());
    ASSERT_EQ (readStringResult.getResult(), tempo_config::ConfigValue("hello, world!"));
}

TEST(ConfigListener, ParseEmptySeq)
{
    auto readSeqResult = tempo_config::read_config_string(R"(
        []
    )");
    ASSERT_TRUE (readSeqResult.isResult());
    ASSERT_EQ (readSeqResult.getResult(), tempo_config::ConfigSeq());
}

TEST(ConfigListener, ParseSeqWithSingleElement)
{
    auto readSeqResult = tempo_config::read_config_string(R"(
        [1]
    )");
    ASSERT_TRUE (readSeqResult.isResult());
    ASSERT_EQ (readSeqResult.getResult(), tempo_config::ConfigSeq({
        tempo_config::ConfigValue("1"),
    }));
}

TEST(ConfigListener, ParseSeqWithMultipleElements)
{
    auto readSeqResult = tempo_config::read_config_string(R"(
        [1, 2, 3]
    )");
    ASSERT_TRUE (readSeqResult.isResult());
    ASSERT_EQ (readSeqResult.getResult(), tempo_config::ConfigSeq({
        tempo_config::ConfigValue("1"),
        tempo_config::ConfigValue("2"),
        tempo_config::ConfigValue("3"),
    }));
}

TEST(ConfigListener, ParseEmptyMap)
{
    auto readMapResult = tempo_config::read_config_string(R"(
        {}
    )");
    ASSERT_TRUE (readMapResult.isResult());
    auto configNode = readMapResult.getResult();
    ASSERT_EQ (tempo_config::ConfigNodeType::kMap, configNode.getNodeType());
    auto configMap = configNode.toMap();
    ASSERT_EQ (tempo_config::ConfigMap(), configMap);
}

TEST(ConfigListener, ParseMapWithSingleEntry)
{
    auto readMapResult = tempo_config::read_config_string(R"(
        {"one": 1}
    )");
    ASSERT_TRUE (readMapResult.isResult());
    auto configNode = readMapResult.getResult();
    ASSERT_EQ (tempo_config::ConfigNodeType::kMap, configNode.getNodeType());
    auto configMap = configNode.toMap();
    ASSERT_EQ (tempo_config::ConfigMap({
        {"one", tempo_config::ConfigValue("1")},
    }), configMap);
}

TEST(ConfigListener, ParseMapWithMultipleEntries)
{
    auto readMapResult = tempo_config::read_config_string(R"(
        {"one": 1, "two": 2, "three": 3}
    )");
    ASSERT_TRUE (readMapResult.isResult());
    auto configNode = readMapResult.getResult();
    ASSERT_EQ (tempo_config::ConfigNodeType::kMap, configNode.getNodeType());
    auto configMap = configNode.toMap();
    ASSERT_EQ (tempo_config::ConfigMap({
        {"one", tempo_config::ConfigValue("1")},
        {"two", tempo_config::ConfigValue("2")},
        {"three", tempo_config::ConfigValue("3")},
    }), configMap);
}

TEST(ConfigListener, ParseDeeplyNestedSeqs)
{
    auto readConfigResult = tempo_config::read_config_string(R"(
    [
        1, [
            2, [
                3, [
                    4, [
                        5, [
                            6, [
                                7, [
                                    8, [ 9 ]
                                ]
                            ]
                        ]
                    ]
                ]
            ]
        ]
    ]
    )");
    ASSERT_TRUE (readConfigResult.isResult());
    auto configNode = readConfigResult.getResult();
    ASSERT_EQ (tempo_config::ConfigNodeType::kSeq, configNode.getNodeType());
    auto configSeq = configNode.toSeq();
    ASSERT_EQ (tempo_config::ConfigSeq({
        tempo_config::ConfigValue("1"),
        tempo_config::ConfigSeq({
            tempo_config::ConfigValue("2"),
            tempo_config::ConfigSeq({
                tempo_config::ConfigValue("3"),
                tempo_config::ConfigSeq({
                    tempo_config::ConfigValue("4"),
                    tempo_config::ConfigSeq({
                        tempo_config::ConfigValue("5"),
                        tempo_config::ConfigSeq({
                            tempo_config::ConfigValue("6"),
                            tempo_config::ConfigSeq({
                                tempo_config::ConfigValue("7"),
                                tempo_config::ConfigSeq({
                                    tempo_config::ConfigValue("8"),
                                    tempo_config::ConfigSeq({
                                        tempo_config::ConfigValue("9")
                                    }),
                                }),
                            }),
                        }),
                    }),
                }),
            }),
        }),
    }), configSeq);
}

TEST(ConfigListener, ParseDeeplyNestedMaps)
{
    auto readConfigResult = tempo_config::read_config_string(R"(
    {
        "one": {
            "value": 1,
            "two": {
                "value": 2,
                "three": {
                    "value": 3,
                    "four": {
                        "value": 4,
                        "five": {
                            "value": 5,
                            "six": {
                                "value": 6,
                                "seven": {
                                    "value": 7,
                                    "eight": {
                                        "value": 8,
                                        "nine": {
                                            "value": 9
                                        }
                                    }
                                }
                            }
                        }
                    }
               }
           }
       }
    }
    )");
    ASSERT_TRUE (readConfigResult.isResult());
    auto configNode = readConfigResult.getResult();
    ASSERT_EQ (tempo_config::ConfigNodeType::kMap, configNode.getNodeType());
    auto configMap = configNode.toMap();
    ASSERT_EQ (tempo_config::ConfigMap({
        {"one", tempo_config::ConfigMap({
            {"value", tempo_config::ConfigValue("1")},
            {"two", tempo_config::ConfigMap({
                {"value", tempo_config::ConfigValue("2")},
                {"three", tempo_config::ConfigMap({
                    {"value", tempo_config::ConfigValue("3")},
                    {"four", tempo_config::ConfigMap({
                        {"value", tempo_config::ConfigValue("4")},
                        {"five", tempo_config::ConfigMap({
                            {"value", tempo_config::ConfigValue("5")},
                            {"six", tempo_config::ConfigMap({
                                {"value", tempo_config::ConfigValue("6")},
                                {"seven", tempo_config::ConfigMap({
                                    {"value", tempo_config::ConfigValue("7")},
                                    {"eight", tempo_config::ConfigMap({
                                        {"value", tempo_config::ConfigValue("8")},
                                        {"nine", tempo_config::ConfigMap({
                                            {"value", tempo_config::ConfigValue("9")},
                                        })},
                                    })},
                                })},
                            })},
                        })},
                    })},
                })},
            })},
        })},
    }), configMap);
}
