#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <tempo_config/base_conversions.h>
#include <tempo_config/container_conversions.h>
#include <tempo_config/parse_config.h>

TEST(ContainerConversions, TestConvertSeqOfInts)
{
    tempo_config::IntegerParser intParser;
    tempo_config::SeqTParser<int> seqIntParser(&intParser);
    std::vector<int> seq;

    auto status = seqIntParser.convertValue(
        tempo_config::ConfigSeq({
            tempo_config::ConfigValue("1"),
            tempo_config::ConfigValue("2"),
            tempo_config::ConfigValue("3"),
        }), seq);
    ASSERT_THAT (seq, testing::ElementsAre(1, 2, 3));
}

TEST(ContainerConversions, TestConvertMapOfStringToInt)
{
    tempo_config::StringParser stringParser;
    tempo_config::IntegerParser intParser;
    tempo_config::MapKVParser<std::string,int> mapOfStringToIntParser(&stringParser, &intParser);
    absl::flat_hash_map<std::string,int> map;

    auto status = mapOfStringToIntParser.convertValue(
        tempo_config::ConfigMap({
            { "one", tempo_config::ConfigValue("1") },
            { "two", tempo_config::ConfigValue("2") },
            { "three", tempo_config::ConfigValue("3") },
        }), map);
    ASSERT_THAT (map, testing::UnorderedElementsAreArray({
        std::pair{"one", 1},
        std::pair{"two", 2},
        std::pair{"three", 3}
    }));
}

TEST(ContainerConversions, TestConvertMapOfStringToSeqOfInt)
{
    tempo_config::StringParser stringParser;
    tempo_config::IntegerParser intParser;
    tempo_config::SeqTParser<int> seqOfIntParser(&intParser);
    tempo_config::MapKVParser<std::string,std::vector<int>> mapOfStringToSeqOfIntParser(&stringParser, &seqOfIntParser);
    absl::flat_hash_map<std::string,std::vector<int>> map;

    auto status = mapOfStringToSeqOfIntParser.convertValue(
        tempo_config::ConfigMap({
            { "one", tempo_config::ConfigSeq({ tempo_config::ConfigValue("1") })
            },
            { "two", tempo_config::ConfigSeq({
                tempo_config::ConfigValue("1"),
                tempo_config::ConfigValue("2") })
            },
            { "three", tempo_config::ConfigSeq({
                tempo_config::ConfigValue("1"),
                tempo_config::ConfigValue("2"),
                tempo_config::ConfigValue("3") })
            },
        }), map);

    ASSERT_THAT (map, testing::UnorderedElementsAreArray({
        std::pair<std::string,std::vector<int>>{"one", {1}},
        std::pair<std::string,std::vector<int>>{"two", {1, 2}},
        std::pair<std::string,std::vector<int>>{"three", {1, 2, 3}},
    }));
}

TEST(ContainerConversions, TestParseMapOfStringToSeqOfInt)
{
    tempo_config::StringParser stringParser;
    tempo_config::IntegerParser intParser;
    tempo_config::SeqTParser<int> seqOfIntParser(&intParser);
    tempo_config::MapKVParser<std::string,std::vector<int>> mapOfStringToSeqOfIntParser(&stringParser, &seqOfIntParser);

    auto config = tempo_config::ConfigMap({
        { "one", tempo_config::ConfigSeq({ tempo_config::ConfigValue("1") })
        },
        { "two", tempo_config::ConfigSeq({
            tempo_config::ConfigValue("1"),
            tempo_config::ConfigValue("2") })
        },
        { "three", tempo_config::ConfigSeq({
            tempo_config::ConfigValue("1"),
            tempo_config::ConfigValue("2"),
            tempo_config::ConfigValue("3") })
        },
    });

    absl::flat_hash_map<std::string,std::vector<int>> result;
    auto status = tempo_config::parse_config(result, mapOfStringToSeqOfIntParser, config);

    ASSERT_TRUE (status.isOk());
    ASSERT_THAT (result, testing::UnorderedElementsAreArray({
        std::pair<std::string,std::vector<int>>{"one", {1}},
        std::pair<std::string,std::vector<int>>{"two", {1, 2}},
        std::pair<std::string,std::vector<int>>{"three", {1, 2, 3}},
    }));
}
