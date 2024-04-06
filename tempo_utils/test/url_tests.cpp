#include <gtest/gtest.h>

#include <tempo_utils/tempo_utils.h>

TEST(Url, TestEmptyUrl)
{
    auto url = tempo_utils::Url::fromString("");
    ASSERT_FALSE (url.validate());

    ASSERT_FALSE (url.isValid());
    ASSERT_FALSE (url.isAbsolute());
    ASSERT_FALSE (url.isRelative());
    ASSERT_FALSE (url.isKnownScheme());
    ASSERT_FALSE (url.isAuthority());
    ASSERT_FALSE (url.isFragment());

    ASSERT_EQ (url.getScheme(), "");
    ASSERT_EQ (url.getUsername(), "");
    ASSERT_EQ (url.getHost(), "");
    ASSERT_EQ (url.getPath(), "");
    ASSERT_EQ (url.getQuery(), "");
    ASSERT_EQ (url.getFragment(), "");

    ASSERT_EQ (url.toString(), "");
}

TEST(Url, TestParseUrlWithAllComponents)
{
    auto url = tempo_utils::Url::fromString("dev.zuri.pkg://core-1.0.0@zuri.dev/core?key=value#Char");
    ASSERT_TRUE (url.validate());

    ASSERT_EQ (url.getScheme(), "dev.zuri.pkg");
    ASSERT_EQ (url.getUsername(), "core-1.0.0");
    ASSERT_EQ (url.getHost(), "zuri.dev");
    ASSERT_EQ (url.getPath(), "/core");
    ASSERT_EQ (url.getQuery(), "key=value");
    ASSERT_EQ (url.getFragment(), "Char");

    ASSERT_EQ (url.toString(), "dev.zuri.pkg://core-1.0.0@zuri.dev/core?key=value#Char");
}

TEST(Url, TestParseUrlWithNoScheme)
{
    auto url = tempo_utils::Url::fromString("//core-1.0.0@zuri.dev/core?key=value#Char");
    ASSERT_TRUE (url.validate());

    ASSERT_FALSE (url.hasScheme());
    ASSERT_EQ (url.getUsername(), "core-1.0.0");
    ASSERT_EQ (url.getHost(), "zuri.dev");
    ASSERT_EQ (url.getPath(), "/core");
    ASSERT_EQ (url.getQuery(), "key=value");
    ASSERT_EQ (url.getFragment(), "Char");

    ASSERT_EQ (url.toString(), "//core-1.0.0@zuri.dev/core?key=value#Char");
}

TEST(Url, TestParseUrlWithPathOnly)
{
    auto url = tempo_utils::Url::fromString("/core");
    ASSERT_TRUE (url.validate());

    ASSERT_FALSE (url.hasScheme());
    ASSERT_FALSE (url.hasAuthority());
    ASSERT_EQ (url.getPath(), "/core");
    ASSERT_FALSE (url.hasQuery());
    ASSERT_FALSE (url.hasFragment());

    ASSERT_EQ (url.toString(), "/core");
}

TEST(Url, TestParseUrlWithPathOnlyLeadingDot)
{
    auto url = tempo_utils::Url::fromString("./core");
    ASSERT_TRUE (url.validate());

    ASSERT_FALSE (url.hasScheme());
    ASSERT_FALSE (url.hasAuthority());
    ASSERT_EQ (url.getPath(), "./core");
    ASSERT_FALSE (url.hasQuery());
    ASSERT_FALSE (url.hasFragment());

    ASSERT_EQ (url.toString(), "./core");
}

TEST(Url, TestParseUrlWithPathOnlyNoLeadingSlash)
{
    auto url = tempo_utils::Url::fromString("core");
    ASSERT_TRUE (url.validate());

    ASSERT_FALSE (url.hasScheme());
    ASSERT_FALSE (url.hasAuthority());
    ASSERT_EQ (url.getPath(), "core");
    ASSERT_FALSE (url.hasQuery());
    ASSERT_FALSE (url.hasFragment());

    ASSERT_EQ (url.toString(), "core");
}

TEST(Url, TestParseUrlWithSchemeAndPathOnlyNoLeadingSlash)
{
    auto url = tempo_utils::Url::fromString("example:path");
    ASSERT_TRUE (url.validate());

    ASSERT_EQ (url.getScheme(), "example");
    ASSERT_FALSE (url.hasAuthority());
    ASSERT_EQ (url.getPath(), "path");
    ASSERT_FALSE (url.hasQuery());
    ASSERT_FALSE (url.hasFragment());

    ASSERT_EQ (url.toString(), "example:path");
}

TEST(Url, TestParseUrlWithNoSchemeOrAuthority)
{
    auto url = tempo_utils::Url::fromString("/core?key=value#Char");
    ASSERT_TRUE (url.validate());

    ASSERT_FALSE (url.hasScheme());
    ASSERT_FALSE (url.hasAuthority());
    ASSERT_EQ (url.getPath(), "/core");
    ASSERT_EQ (url.getQuery(), "key=value");
    ASSERT_EQ (url.getFragment(), "Char");

    ASSERT_EQ (url.toString(), "/core?key=value#Char");
}

TEST(Url, TestParseUrlWithNoSchemeOrAuthorityOrPath)
{
    auto url = tempo_utils::Url::fromString("?key=value#Char");
    ASSERT_TRUE (url.validate());

    ASSERT_FALSE (url.hasScheme());
    ASSERT_FALSE (url.hasAuthority());
    ASSERT_FALSE (url.hasPath());
    ASSERT_EQ (url.getQuery(), "key=value");
    ASSERT_EQ (url.getFragment(), "Char");

    ASSERT_EQ (url.toString(), "?key=value#Char");
}

//TEST(Url, TestMergeBaseWithRelativePathOnly)
//{
//    auto base = tempo_utils::Url::fromString("dev.zuri.pkg://core-1.0.0@zuri.dev/core");
//    TU_CONSOLE_ERR << "base: " << base.toString();
//    auto relative = tempo_utils::Url::fromString("/module");
//    TU_CONSOLE_ERR << "relative: " << relative.toString();
//
//    auto result = base / relative;
//    TU_CONSOLE_ERR << "result: " << result.toString();
//    ASSERT_TRUE (result.validate());
//
//    ASSERT_EQ (result.getScheme(), "dev.zuri.pkg:");
//    ASSERT_EQ (result.getUsername(), "core-1.0.0");
//    ASSERT_EQ (result.getHost(), "zuri.dev");
//    ASSERT_EQ (result.getPath(), "/core/module");
//    ASSERT_FALSE (result.hasQuery());
//    ASSERT_FALSE (result.hasFragment());
//
//    ASSERT_EQ (result.toString(), "dev.zuri.pkg://core-1.0.0@zuri.dev/core/module");
//}

TEST(Url, TestParseUrlWithRelativeFragmentOnly)
{
    auto url = tempo_utils::Url::fromString("#Char");
    ASSERT_TRUE (url.validate());

    ASSERT_FALSE (url.hasScheme());
    ASSERT_FALSE (url.hasAuthority());
    ASSERT_FALSE (url.hasPath());
    ASSERT_FALSE (url.hasQuery());
    ASSERT_EQ (url.getFragment(), "Char");

    ASSERT_EQ (url.toString(), "#Char");
}

//TEST(Url, TestConstructUrlWithFileScheme)
//{
//    auto url = tempo_utils::Url::fromComponents("test", "", "", "", "", "/foo.txt", {}, {});
//    ASSERT_TRUE (url.validate());
//    ASSERT_EQ (url.toString(), "test:///foo.txt");
//}
//
//TEST(Url, TestMergeBaseWithRelativeFragmentOnly)
//{
//    auto base = tempo_utils::Url::fromString("dev.zuri.pkg://core-1.0.0@zuri.dev/core");
//    auto relative = tempo_utils::Url::fromString("#Char");
//
//    auto result = base / relative;
//    ASSERT_TRUE (result.validate());
//
//    ASSERT_EQ (result.getScheme(), "dev.zuri.pkg:");
//    ASSERT_EQ (result.getUsername(), "core-1.0.0");
//    ASSERT_EQ (result.getHost(), "zuri.dev");
//    ASSERT_EQ (result.getPath(), "/core");
//    ASSERT_FALSE (result.hasQuery());
//    ASSERT_EQ (result.getFragment(), "#Char");
//
//    ASSERT_EQ (result.toString(), "dev.zuri.pkg://core-1.0.0@zuri.dev/core#Char");
//}

TEST(Url, TestConstructUrlFromAbsolute)
{
    auto url = tempo_utils::Url::fromAbsolute("dev.zuri.pkg", "test-0.0.1@zuri.dev");

    auto authority = url.toAuthority();
    ASSERT_EQ (authority.getUsername(), "test-0.0.1");
    ASSERT_EQ (authority.getHost(), "zuri.dev");
    ASSERT_EQ (authority.getHostAndPort(), "zuri.dev");

    ASSERT_EQ (url.toString(), "dev.zuri.pkg://test-0.0.1@zuri.dev");
}

TEST(Url, TestConstructUrlFromAuthority)
{
    auto url = tempo_utils::Url::fromAuthority("test-0.0.1@zuri.dev");

    auto authority = url.toAuthority();
    ASSERT_EQ (authority.getUsername(), "test-0.0.1");
    ASSERT_EQ (authority.getHost(), "zuri.dev");
    ASSERT_EQ (authority.getHostAndPort(), "zuri.dev");

    ASSERT_EQ (url.toString(), "//test-0.0.1@zuri.dev");
}

TEST(Url, TestConstructUrlFromOrigin)
{
    auto url = tempo_utils::Url::fromOrigin("dev.zuri.pkg://zuri.dev");

    auto origin = url.toOrigin();
    ASSERT_EQ (origin.getScheme(), "dev.zuri.pkg");
    ASSERT_EQ (origin.getHost(), "zuri.dev");
    ASSERT_EQ (origin.getHostAndPort(), "zuri.dev");

    ASSERT_EQ (url.toString(), "dev.zuri.pkg://zuri.dev");
}

TEST(Url, TestConstructUrlFromFilesystemPath)
{
    std::filesystem::path path("/foo/bar/baz");
    auto url = tempo_utils::Url::fromFilesystemPath(path);

    ASSERT_EQ (url.getScheme(), "file");
    ASSERT_EQ (url.getPath(), "/foo/bar/baz");

    ASSERT_EQ (url.toString(), "file:///foo/bar/baz");
}

TEST(Url, TestConstructUrlFromFilesystemPathWithRootName)
{
    GTEST_SKIP();
    // skipping this test for now since it requires windows

    std::filesystem::path path("C:/foo/bar/baz");
    auto url = tempo_utils::Url::fromFilesystemPath(path);

    ASSERT_EQ (url.getScheme(), "file");
    ASSERT_EQ (url.getPath(), "/C:/foo/bar/baz");

    ASSERT_EQ (url.toString(), "file:///C:/foo/bar/baz");
}

TEST(Url, TestTraverseAuthorityUrl)
{
    auto baseUrl = tempo_utils::Url::fromString("https://test.com/foo/bar");
    auto url = baseUrl.traverse(tempo_utils::UrlPathPart("baz"));

    ASSERT_EQ (url.toString(), "https://test.com/foo/bar/baz");
}

TEST(Url, TestTraverseOriginUrl)
{
    auto baseUrl = tempo_utils::Url::fromString("//authority/foo/bar");
    auto url = baseUrl.traverse(tempo_utils::UrlPathPart("baz"));

    ASSERT_EQ (url.toString(), "//authority/foo/bar/baz");
}

TEST(Url, TestTraverseRelativeUrl)
{
    auto baseUrl = tempo_utils::Url::fromString("/foo/bar");
    auto url = baseUrl.traverse(tempo_utils::UrlPathPart("baz"));

    ASSERT_EQ (url.toString(), "/foo/bar/baz");
}

TEST(Url, TestUpdateUrlScheme)
{
    auto urlNoScheme = tempo_utils::Url::fromString("//localhost/foo");

    ASSERT_EQ ("http://localhost/foo", urlNoScheme.withScheme("http").toString());

    auto urlWithScheme = tempo_utils::Url::fromString("http://localhost/foo");
    ASSERT_EQ ("https://localhost/foo", urlWithScheme.withScheme("https").toString());
    ASSERT_EQ ("//localhost/foo", urlWithScheme.withScheme("").toString());
}

TEST(Url, TestUpdateUrlPath)
{
    auto urlNoPath = tempo_utils::Url::fromString("http://localhost");

    ASSERT_EQ ("http://localhost/foo", urlNoPath.withPath("/foo").toString());

    auto urlWithPath = tempo_utils::Url::fromString("http://localhost/foo");
    ASSERT_EQ ("http://localhost/bar", urlWithPath.withPath("/bar").toString());
    ASSERT_EQ ("http://localhost", urlWithPath.withPath("").toString());
}

TEST(Url, TestUpdateUrlQuery)
{
    auto urlNoQuery = tempo_utils::Url::fromString("http://localhost/foo");

    ASSERT_EQ ("http://localhost/foo?added", urlNoQuery.withQuery("added").toString());

    auto urlWithQuery = tempo_utils::Url::fromString("http://localhost/foo?existing");
    ASSERT_EQ ("http://localhost/foo?changed", urlWithQuery.withQuery("changed").toString());
    ASSERT_EQ ("http://localhost/foo", urlWithQuery.withQuery("").toString());
}

TEST(Url, TestUpdateUrlFragment)
{
    auto urlNoFragment = tempo_utils::Url::fromString("http://localhost/foo");

    ASSERT_EQ ("http://localhost/foo#added", urlNoFragment.withFragment("added").toString());

    auto urlWithFragment = tempo_utils::Url::fromString("http://localhost/foo#existing");
    ASSERT_EQ ("http://localhost/foo#changed", urlWithFragment.withFragment("changed").toString());
    ASSERT_EQ ("http://localhost/foo", urlWithFragment.withFragment("").toString());
}
