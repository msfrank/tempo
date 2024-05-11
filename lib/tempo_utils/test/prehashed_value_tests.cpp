
#include <gtest/gtest.h>

#include <tempo_utils/prehashed_value.h>
#include <absl/container/flat_hash_map.h>

TEST(PrehashedValue, EmptyValuesHaveTheSameHash)
{
    tempo_utils::PrehashedValue<std::string> v1;
    tempo_utils::PrehashedValue<std::string> v2;
    ASSERT_EQ (absl::HashOf(v1), absl::HashOf(v2));
}

TEST(PrehashedValue, EqualValuesHaveTheSameHash)
{
    tempo_utils::PrehashedValue v1(std::string{"hello world!"});
    tempo_utils::PrehashedValue v2(std::string{"hello world!"});
    ASSERT_EQ (absl::HashOf(v1), absl::HashOf(v2));
}

TEST(PrehashedValue, EmptyAndNonEmptyValuesHaveDifferentHashes)
{
    tempo_utils::PrehashedValue<std::string> v1;
    tempo_utils::PrehashedValue v2(std::string{"hello world!"});
    ASSERT_NE (absl::HashOf(v1), absl::HashOf(v2));
}

TEST(PrehashedValue, EmptyValueInvariants)
{
    tempo_utils::PrehashedValue<std::string> v1;
    ASSERT_TRUE (v1.isEmpty());
    ASSERT_FALSE (v1.nonEmpty());
}

TEST(PrehashedValue, NonEmptyValueInvariants)
{
    tempo_utils::PrehashedValue v1(std::string{"hello world!"});
    ASSERT_FALSE (v1.isEmpty());
    ASSERT_TRUE (v1.nonEmpty());
    ASSERT_EQ (12, v1->size());
    ASSERT_EQ ("hello world!", *v1);
}

TEST(PrehashedValue, ValuesUsedAsHashMapKeys)
{
    absl::flat_hash_map<tempo_utils::PrehashedValue<std::string>, int> map;

    map[tempo_utils::PrehashedValue(std::string{"one"})] = 1;
    map[tempo_utils::PrehashedValue(std::string{"two"})] = 2;
    map[tempo_utils::PrehashedValue(std::string{"three"})] = 3;

    ASSERT_EQ (1, map.at(tempo_utils::PrehashedValue(std::string{"one"})));
    ASSERT_EQ (2, map.at(tempo_utils::PrehashedValue(std::string{"two"})));
    ASSERT_EQ (3, map.at(tempo_utils::PrehashedValue(std::string{"three"})));
}

TEST(PrehashedValue, ConstructViaArgumentForwarding)
{
    auto v1 = tempo_utils::make_prehashed<std::string>("hello world!");
    ASSERT_TRUE (v1.nonEmpty());
    ASSERT_EQ ("hello world!", *v1);
}
