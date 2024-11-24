#include <gtest/gtest.h>

#include <absl/strings/escaping.h>

#include <tempo_security/sha256_hash.h>

#define TEST_STRING     "hello, world!"
#define TEST_HASH       "68e656b251e67e8358bef8483ab0d51c6619f3e7a1a9f0e75838d41ff368f728"

TEST(Sha256Hash, GenerateHashValue)
{
    auto result = tempo_security::Sha256Hash::hash(TEST_STRING);
    ASSERT_FALSE (result.empty());
    auto hex = absl::BytesToHexString(result);
    ASSERT_EQ (TEST_HASH, hex);
}

TEST(Sha256Hash, GenerateHashValueSingleAdd)
{
    tempo_security::Sha256Hash hasher;
    auto nodata = hasher.getResult();
    ASSERT_EQ ("", nodata);
    hasher.addData(TEST_STRING);
    auto result = hasher.getResult();
    ASSERT_FALSE (result.empty());
    auto hex = absl::BytesToHexString(result);
    ASSERT_EQ (TEST_HASH, hex);
}

TEST(Sha256Hash, GenerateHashValueMultipleAdd)
{
    tempo_security::Sha256Hash hasher;
    auto nodata = hasher.getResult();
    ASSERT_EQ ("", nodata);
    hasher.addData("hello, ");
    hasher.addData("world");
    hasher.addData("!");
    auto result = hasher.getResult();
    ASSERT_FALSE (result.empty());
    auto hex = absl::BytesToHexString(result);
    ASSERT_EQ (TEST_HASH, hex);
}
