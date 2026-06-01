
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <tempo_utils/hash_array_mapped_trie.h>

class HashArrayMappedTrie : public ::testing::Test {};

TEST_F(HashArrayMappedTrie, ConstructEmpty)
{
    tempo_utils::HashArrayMappedTrie<std::string,std::string> trie;

    ASSERT_TRUE (trie.isEmpty());
}

TEST_F(HashArrayMappedTrie, ConstructFromKeyValue)
{
    auto trie = tempo_utils::HashArrayMappedTrie<std::string,std::string>::of("key", "value");

    ASSERT_FALSE (trie.isEmpty());
    ASSERT_EQ (1, trie.numEntries());
}

TEST_F(HashArrayMappedTrie, ConstructFromInitializerList)
{
    tempo_utils::HashArrayMappedTrie<std::string,std::string> trie({
            { "1", "one"},
            { "2", "two"},
            { "3", "three"},
            { "4", "four"},
            { "5", "five"},
    });

    ASSERT_FALSE (trie.isEmpty());
    ASSERT_EQ (5, trie.numEntries());
}

TEST_F(HashArrayMappedTrie, ConstructFromVector)
{
    std::vector<std::pair<std::string,std::string>> entries = {
        { "1", "one"},
        { "2", "two"},
        { "3", "three"},
        { "4", "four"},
        { "5", "five"},
    };
    tempo_utils::HashArrayMappedTrie trie(entries);

    ASSERT_FALSE (trie.isEmpty());
    ASSERT_EQ (5, trie.numEntries());
}

TEST_F(HashArrayMappedTrie, ConstructFromMap)
{
    std::map<std::string,std::string> entries = {
        { "1", "one"},
        { "2", "two"},
        { "3", "three"},
        { "4", "four"},
        { "5", "five"},
    };
    auto trie = tempo_utils::HashArrayMappedTrie<std::string,std::string>::fromMap(entries.begin(), entries.end());

    ASSERT_FALSE (trie.isEmpty());
    ASSERT_EQ (5, trie.numEntries());
}

TEST_F(HashArrayMappedTrie, ContainsReturnsTrueWhenKeyIsPresent)
{
    tempo_utils::HashArrayMappedTrie<std::string,std::string> trie({
            { "1", "one"},
            { "2", "two"},
            { "3", "three"},
            { "4", "four"},
            { "5", "five"},
    });

    ASSERT_TRUE (trie.contains("1"));
    ASSERT_TRUE (trie.contains("2"));
    ASSERT_TRUE (trie.contains("3"));
    ASSERT_TRUE (trie.contains("4"));
    ASSERT_TRUE (trie.contains("5"));
}

TEST_F(HashArrayMappedTrie, ContainsReturnsFalseWhenKeyIsAbsent)
{
    tempo_utils::HashArrayMappedTrie<std::string,std::string> trie({
            { "1", "one"},
            { "2", "two"},
            { "3", "three"},
            { "4", "four"},
            { "5", "five"},
    });

    ASSERT_FALSE (trie.contains("0"));
    ASSERT_FALSE (trie.contains("one"));
    ASSERT_FALSE (trie.contains("foo"));
    ASSERT_FALSE (trie.contains("abcdefghijklmnopqrstuvwxyz"));
}

TEST_F(HashArrayMappedTrie, NewEntryInsertedOnUpdate)
{
    tempo_utils::HashArrayMappedTrie<std::string,std::string> trie({
            { "1", "one"},
            { "2", "two"},
            { "3", "three"},
    });
    ASSERT_EQ (3, trie.numEntries());

    trie = trie.update("4", "four");
    ASSERT_EQ (4, trie.numEntries());
    ASSERT_TRUE (trie.contains("1"));
    ASSERT_TRUE (trie.contains("2"));
    ASSERT_TRUE (trie.contains("3"));
    ASSERT_TRUE (trie.contains("4"));
}

TEST_F(HashArrayMappedTrie, ExistingEntryReplacedOnUpdate)
{
    tempo_utils::HashArrayMappedTrie<std::string,std::string> trie({
            { "1", "one"},
            { "2", "two"},
            { "3", "three"},
    });
    ASSERT_EQ (3, trie.numEntries());

    trie = trie.update("2", "dos");
    ASSERT_EQ (3, trie.numEntries());
    ASSERT_TRUE (trie.contains("1"));
    ASSERT_TRUE (trie.contains("2"));
    ASSERT_TRUE (trie.contains("3"));
}

TEST_F(HashArrayMappedTrie, ExistingEntryRemoved)
{
    tempo_utils::HashArrayMappedTrie<std::string,std::string> trie({
            { "1", "one"},
            { "2", "two"},
            { "3", "three"},
    });
    ASSERT_EQ (3, trie.numEntries());

    trie = trie.remove("2");
    ASSERT_EQ (2, trie.numEntries());
    ASSERT_TRUE (trie.contains("1"));
    ASSERT_TRUE (trie.contains("3"));
}
