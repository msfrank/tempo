#include <gtest/gtest.h>

#include <tempo_utils/url_authority.h>
#include <tempo_utils/url.h>

TEST(UrlAuthority, EmptyAuthority) {
    auto authority = tempo_utils::UrlAuthority::fromString("");

    ASSERT_FALSE (authority.isValid());
    ASSERT_TRUE (authority.isEmpty());
    ASSERT_FALSE (authority.hasHost());
    ASSERT_FALSE (authority.hasPort());
    ASSERT_FALSE (authority.hasCredentials());
    ASSERT_FALSE (authority.hasUsername());
    ASSERT_FALSE (authority.hasPassword());

    ASSERT_EQ (authority.getHost(), "");
    ASSERT_EQ (authority.getPort(), "");
    ASSERT_EQ (authority.getHostAndPort(), "");
    ASSERT_EQ (authority.getUsername(), "");
    ASSERT_EQ (authority.getPassword(), "");

    ASSERT_EQ (authority.toString(), "");
}

TEST(UrlAuthority, ParseAuthorityHost)
{
    auto authority = tempo_utils::UrlAuthority::fromString("foo.com");

    ASSERT_TRUE (authority.isValid());
    ASSERT_FALSE (authority.isEmpty());
    ASSERT_TRUE (authority.hasHost());
    ASSERT_FALSE (authority.hasPort());
    ASSERT_FALSE (authority.hasCredentials());
    ASSERT_FALSE (authority.hasUsername());
    ASSERT_FALSE (authority.hasPassword());

    ASSERT_EQ (authority.getHost(), "foo.com");
    ASSERT_EQ (authority.getPort(), "");
    ASSERT_EQ (authority.getHostAndPort(), "foo.com");
    ASSERT_EQ (authority.getUsername(), "");
    ASSERT_EQ (authority.getPassword(), "");

    ASSERT_EQ (authority.toString(), "foo.com");
}

TEST(UrlAuthority, ParseAuthorityHostAndPort)
{
    auto authority = tempo_utils::UrlAuthority::fromString("foo.com:443");

    ASSERT_TRUE (authority.isValid());
    ASSERT_FALSE (authority.isEmpty());
    ASSERT_TRUE (authority.hasHost());
    ASSERT_TRUE (authority.hasPort());
    ASSERT_FALSE (authority.hasCredentials());
    ASSERT_FALSE (authority.hasUsername());
    ASSERT_FALSE (authority.hasPassword());

    ASSERT_EQ (authority.getHost(), "foo.com");
    ASSERT_EQ (authority.getPort(), "443");
    ASSERT_EQ (authority.getHostAndPort(), "foo.com:443");
    ASSERT_EQ (authority.getUsername(), "");
    ASSERT_EQ (authority.getPassword(), "");

    ASSERT_EQ (authority.toString(), "foo.com:443");
}

TEST(UrlAuthority, ParseAuthorityUsernameHostAndPort)
{
    auto authority = tempo_utils::UrlAuthority::fromString("user@foo.com:443");

    ASSERT_TRUE (authority.isValid());
    ASSERT_FALSE (authority.isEmpty());
    ASSERT_TRUE (authority.hasHost());
    ASSERT_TRUE (authority.hasPort());
    ASSERT_TRUE (authority.hasCredentials());
    ASSERT_TRUE (authority.hasUsername());
    ASSERT_FALSE (authority.hasPassword());

    ASSERT_EQ (authority.getHost(), "foo.com");
    ASSERT_EQ (authority.getPort(), "443");
    ASSERT_EQ (authority.getHostAndPort(), "foo.com:443");
    ASSERT_EQ (authority.getUsername(), "user");
    ASSERT_EQ (authority.getPassword(), "");

    ASSERT_EQ (authority.toString(), "user@foo.com:443");
}

TEST(UrlAuthority, ParseAuthorityUsernamePasswordHostAndPort)
{
    auto authority = tempo_utils::UrlAuthority::fromString("user:pass@foo.com:443");

    ASSERT_TRUE (authority.isValid());
    ASSERT_FALSE (authority.isEmpty());
    ASSERT_TRUE (authority.hasHost());
    ASSERT_TRUE (authority.hasPort());
    ASSERT_TRUE (authority.hasCredentials());
    ASSERT_TRUE (authority.hasUsername());
    ASSERT_TRUE (authority.hasPassword());

    ASSERT_EQ (authority.getHost(), "foo.com");
    ASSERT_EQ (authority.getPort(), "443");
    ASSERT_EQ (authority.getHostAndPort(), "foo.com:443");
    ASSERT_EQ (authority.getUsername(), "user");
    ASSERT_EQ (authority.getPassword(), "pass");

    ASSERT_EQ (authority.toString(), "user:pass@foo.com:443");
}

TEST(UrlAuthority, CompareAuthorityEquals)
{
    auto authority = tempo_utils::UrlAuthority::fromString("user:pass@foo.com:443");
    ASSERT_EQ (authority, authority);
    auto url = tempo_utils::Url::fromString("http://user:pass@foo.com:443");
    ASSERT_EQ (authority, url.toAuthority());
}
