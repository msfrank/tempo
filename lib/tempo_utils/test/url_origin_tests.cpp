#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_utils/url_origin.h>

TEST(UrlOrigin, ParseEmptyOrigin)
{
    auto origin = tempo_utils::UrlOrigin::fromString("");

    ASSERT_FALSE (origin.isValid());
    ASSERT_TRUE (origin.isEmpty());
    ASSERT_FALSE (origin.hasScheme());
    ASSERT_FALSE (origin.hasHost());
    ASSERT_FALSE (origin.hasPort());

    ASSERT_EQ (origin.getScheme(), "");
    ASSERT_EQ (origin.getHost(), "");
    ASSERT_EQ (origin.getPort(), "");
    ASSERT_EQ (origin.getHostAndPort(), "");

    ASSERT_EQ (origin.toString(), "");
}

TEST(UrlOrigin, ParseOriginHost)
{
    auto origin = tempo_utils::UrlOrigin::fromString("//www.foo.com");

    ASSERT_TRUE (origin.isValid());
    ASSERT_FALSE (origin.isEmpty());
    ASSERT_FALSE (origin.hasScheme());
    ASSERT_TRUE (origin.hasHost());
    ASSERT_FALSE (origin.hasPort());

    ASSERT_EQ (origin.getScheme(), "");
    ASSERT_EQ (origin.getHost(), "www.foo.com");
    ASSERT_EQ (origin.getPort(), "");
    ASSERT_EQ (origin.getHostAndPort(), "www.foo.com");

    ASSERT_EQ (origin.toString(), "//www.foo.com");
}

TEST(UrlOrigin, ParseOriginHostAndPort)
{
    auto origin = tempo_utils::UrlOrigin::fromString("//www.foo.com:443");

    ASSERT_TRUE (origin.isValid());
    ASSERT_FALSE (origin.isEmpty());
    ASSERT_FALSE (origin.hasScheme());
    ASSERT_TRUE (origin.hasHost());
    ASSERT_TRUE (origin.hasPort());

    ASSERT_EQ (origin.getScheme(), "");
    ASSERT_EQ (origin.getHost(), "www.foo.com");
    ASSERT_EQ (origin.getPort(), "443");
    ASSERT_EQ (origin.getHostAndPort(), "www.foo.com:443");

    ASSERT_EQ (origin.toString(), "//www.foo.com:443");
}

TEST(UrlOrigin, ParseOriginSchemeHostAndPort)
{
    auto origin = tempo_utils::UrlOrigin::fromString("https://www.foo.com:443");

    ASSERT_TRUE (origin.isValid());
    ASSERT_FALSE (origin.isEmpty());
    ASSERT_TRUE (origin.hasScheme());
    ASSERT_TRUE (origin.hasHost());
    ASSERT_TRUE (origin.hasPort());

    ASSERT_EQ (origin.getScheme(), "https");
    ASSERT_EQ (origin.getHost(), "www.foo.com");
    ASSERT_EQ (origin.getPort(), "443");
    ASSERT_EQ (origin.getHostAndPort(), "www.foo.com:443");

    ASSERT_EQ (origin.toString(), "https://www.foo.com:443");
}
