#include <gtest/gtest.h>

#include <tempo_utils/tempo_utils.h>

TEST(UrlResolve, ResolveSinglePathSegment)
{
    auto base = tempo_utils::Url::fromString("http://a/b/c/d;p?q");
    auto rel = tempo_utils::Url::fromRelative("g");

    auto resolved = base.resolve(rel);
    ASSERT_EQ ("http://a/b/c/g", resolved.toString());
}

TEST(UrlResolve, ResolveMultiplePathSegments)
{
    auto base = tempo_utils::Url::fromString("http://a/b/c/d;p?q");
    auto rel = tempo_utils::Url::fromRelative("g/h/i");

    auto resolved = base.resolve(rel);
    ASSERT_EQ ("http://a/b/c/g/h/i", resolved.toString());
}

TEST(UrlResolve, ResolveSingleParentPathSegment)
{
    auto base = tempo_utils::Url::fromString("http://a/b/c/d;p?q");
    auto rel = tempo_utils::Url::fromRelative("../g");

    auto resolved = base.resolve(rel);
    ASSERT_EQ ("http://a/b/g", resolved.toString());
}

TEST(UrlResolve, ResolveMultipleParentPathSegments)
{
    auto base = tempo_utils::Url::fromString("http://a/b/c/d;p?q");
    auto rel = tempo_utils::Url::fromRelative("../../i");

    auto resolved = base.resolve(rel);
    ASSERT_EQ ("http://a/i", resolved.toString());
}

TEST(UrlResolve, ResolveExtraParentPathSegments)
{
    auto base = tempo_utils::Url::fromString("http://a/b/c/d;p?q");
    auto rel = tempo_utils::Url::fromRelative("../../../../i");

    auto resolved = base.resolve(rel);
    ASSERT_EQ ("http://a/../../i", resolved.toString());
}

TEST(UrlResolve, BaseMustHaveScheme)
{
    auto baseMissingScheme = tempo_utils::Url::fromString("//a/b/c/d;p?q");
    ASSERT_TRUE (baseMissingScheme.isValid());
    auto baseMissingOrigin = tempo_utils::Url::fromString("/b/c/d;p?q");
    ASSERT_TRUE (baseMissingOrigin.isValid());

    auto rel = tempo_utils::Url::fromRelative("g");
    ASSERT_FALSE (baseMissingScheme.resolve(rel).isValid());
    ASSERT_FALSE (baseMissingOrigin.resolve(rel).isValid());
}
