#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <tempo_config/base_conversions.h>

TEST(BaseConversions, TestConvertBoolean)
{
    tempo_config::BooleanParser parser;
    bool b;
    auto status = parser.convertValue(tempo_config::ConfigValue("true"), b);
    ASSERT_TRUE (status.isOk());
    ASSERT_TRUE (b);
}

TEST(BaseConversions, TestConvertInteger)
{
    tempo_config::IntegerParser parser;
    int i;
    auto status = parser.convertValue(tempo_config::ConfigValue("42"), i);
    ASSERT_TRUE (status.isOk());
    ASSERT_EQ (42, i);
}

TEST(BaseConversions, TestConvertLong)
{
    tempo_config::LongParser parser;
    tu_int64 i64;
    auto status = parser.convertValue(tempo_config::ConfigValue("1000"), i64);
    ASSERT_TRUE (status.isOk());
    ASSERT_EQ (1000LL, i64);
}

TEST(BaseConversions, TestConvertFloat)
{
    tempo_config::FloatParser parser;
    double dbl;
    auto status = parser.convertValue(tempo_config::ConfigValue("4.5"), dbl);
    ASSERT_TRUE (status.isOk());
    ASSERT_DOUBLE_EQ (4.5, dbl);
}

TEST(BaseConversions, TestConvertString)
{
    tempo_config::StringParser parser;
    std::string str;
    auto status = parser.convertValue(tempo_config::ConfigValue("hello world!"), str);
    ASSERT_TRUE (status.isOk());
    ASSERT_EQ ("hello world!", str);
}

TEST(BaseConversions, TestConvertPath)
{
    tempo_config::PathParser parser;
    std::filesystem::path path;
    auto status = parser.convertValue(tempo_config::ConfigValue("/foo/bar"), path);
    ASSERT_TRUE (status.isOk());
    ASSERT_EQ (std::filesystem::path("/foo/bar"), path);
}

TEST(BaseConversions, TestConvertUrl)
{
    tempo_config::UrlParser parser;
    tempo_utils::Url url;
    auto status = parser.convertValue(tempo_config::ConfigValue("http://foo.com/bar"), url);
    ASSERT_TRUE (status.isOk());
    ASSERT_EQ (tempo_utils::Url::fromString("http://foo.com/bar"), url);
}

TEST(BaseConversions, TestConvertUrlPath)
{
    tempo_config::UrlPathParser parser;
    tempo_utils::UrlPath path;
    auto status = parser.convertValue(tempo_config::ConfigValue("/foo/bar"), path);
    ASSERT_TRUE (status.isOk());
    ASSERT_EQ (tempo_utils::UrlPath::fromString("/foo/bar"), path);
}
