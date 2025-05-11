#include <gtest/gtest.h>

#include <tempo_utils/tempo_utils.h>

TEST(UrlPath, TestParseEmptyString)
{
    auto urlPath = tempo_utils::UrlPath::fromString("");
    ASSERT_TRUE (urlPath.isValid());

    ASSERT_TRUE (urlPath.isEmpty());
    ASSERT_FALSE (urlPath.isAbsolute());
    ASSERT_EQ (0, urlPath.numParts());

    ASSERT_EQ ("", urlPath.toString());
}

TEST(UrlPath, TestParseSingleSlash)
{
    auto urlPath = tempo_utils::UrlPath::fromString("/");
    ASSERT_TRUE (urlPath.isValid());

    ASSERT_TRUE (urlPath.isEmpty());
    ASSERT_TRUE (urlPath.isAbsolute());
    ASSERT_EQ (0, urlPath.numParts());

    ASSERT_EQ ("/", urlPath.toString());
}

TEST(UrlPath, TestParseSingleDot)
{
    auto urlPath = tempo_utils::UrlPath::fromString(".");
    ASSERT_TRUE (urlPath.isValid());

    ASSERT_FALSE (urlPath.isEmpty());
    ASSERT_FALSE (urlPath.isAbsolute());
    ASSERT_EQ (1, urlPath.numParts());
    ASSERT_EQ (".", urlPath.partView(0));

    ASSERT_EQ (".", urlPath.toString());
}

TEST(UrlPath, TestParseDoubleDot)
{
    auto urlPath = tempo_utils::UrlPath::fromString("..");
    ASSERT_TRUE (urlPath.isValid());

    ASSERT_FALSE (urlPath.isEmpty());
    ASSERT_FALSE (urlPath.isAbsolute());
    ASSERT_EQ (1, urlPath.numParts());
    ASSERT_EQ ("..", urlPath.partView(0));

    ASSERT_EQ ("..", urlPath.toString());
}

TEST(UrlPath, TestParseAbsoluteInputWithSingleSegment)
{
    auto urlPath = tempo_utils::UrlPath::fromString("/test");
    ASSERT_TRUE (urlPath.isValid());

    ASSERT_FALSE(urlPath.isEmpty());
    ASSERT_EQ (1, urlPath.numParts());
    ASSERT_EQ ("test", urlPath.partView(0));
}

TEST(UrlPath, TestParseAbsoluteInputWithMultipleSegments)
{
    auto urlPath = tempo_utils::UrlPath::fromString("/foo/bar/baz/qux");
    ASSERT_TRUE (urlPath.isValid());

    ASSERT_FALSE(urlPath.isEmpty());
    ASSERT_EQ (4, urlPath.numParts());
    ASSERT_EQ ("foo", urlPath.partView(0));
    ASSERT_EQ ("bar", urlPath.partView(1));
    ASSERT_EQ ("baz", urlPath.partView(2));
    ASSERT_EQ ("qux", urlPath.partView(3));
}

TEST(UrlPath, TestParseRelativeInputWithSingleSegment)
{
    auto urlPath = tempo_utils::UrlPath::fromString("test");
    ASSERT_TRUE (urlPath.isValid());

    ASSERT_FALSE(urlPath.isEmpty());
    ASSERT_EQ (1, urlPath.numParts());
    ASSERT_EQ ("test", urlPath.partView(0));
}

TEST(UrlPath, TestParseRelativeInputWithMultipleSegments)
{
    auto urlPath = tempo_utils::UrlPath::fromString("foo/bar/baz/qux");
    ASSERT_TRUE (urlPath.isValid());

    ASSERT_FALSE(urlPath.isEmpty());
    ASSERT_EQ (4, urlPath.numParts());
    ASSERT_EQ ("foo", urlPath.partView(0));
    ASSERT_EQ ("bar", urlPath.partView(1));
    ASSERT_EQ ("baz", urlPath.partView(2));
    ASSERT_EQ ("qux", urlPath.partView(3));
}

TEST(UrlPath, TestExtractHead)
{
    auto urlPath = tempo_utils::UrlPath::fromString("/foo/bar/baz/qux");
    auto pathPart = urlPath.getHead();

    ASSERT_EQ ("foo", pathPart.getPart());
}

TEST(UrlPath, TestExtractLast)
{
    auto urlPath = tempo_utils::UrlPath::fromString("/foo/bar/baz/qux");
    auto pathPart = urlPath.getLast();

    ASSERT_EQ ("qux", pathPart.getPart());
}

TEST(UrlPath, TestExtractInitMultipleSegments)
{
    auto descendentPath = tempo_utils::UrlPath::fromString("/foo/bar/baz/qux");
    auto urlPath = descendentPath.getInit();

    ASSERT_FALSE(urlPath.isEmpty());
    ASSERT_EQ (3, urlPath.numParts());
    ASSERT_EQ ("foo", urlPath.partView(0));
    ASSERT_EQ ("bar", urlPath.partView(1));
    ASSERT_EQ ("baz", urlPath.partView(2));
    ASSERT_EQ ("/foo/bar/baz", urlPath.pathView());
}

TEST(UrlPath, TestExtractInitSingleSegment)
{
    auto descendentPath = tempo_utils::UrlPath::fromString("/foo");
    auto urlPath = descendentPath.getInit();

    ASSERT_TRUE(urlPath.isValid());
    ASSERT_TRUE(urlPath.isEmpty());
    ASSERT_EQ (0, urlPath.numParts());
    ASSERT_EQ ("/", urlPath.pathView());
}

TEST(UrlPath, TestExtractInitEmpty)
{
    auto descendentPath = tempo_utils::UrlPath::fromString("/");
    auto urlPath = descendentPath.getInit();

    ASSERT_TRUE(urlPath.isValid());
    ASSERT_TRUE(urlPath.isEmpty());
    ASSERT_EQ (0, urlPath.numParts());
    ASSERT_EQ ("/", urlPath.pathView());
}

TEST(UrlPath, TestExtractInitInvalid)
{
    auto descendentPath = tempo_utils::UrlPath();
    auto urlPath = descendentPath.getInit();

    ASSERT_FALSE(urlPath.isValid());
    ASSERT_TRUE(urlPath.isEmpty());
    ASSERT_EQ (0, urlPath.numParts());
    ASSERT_EQ ("", urlPath.pathView());
}

TEST(UrlPath, TestExtractTailMultipleSegments)
{
    auto descendentPath = tempo_utils::UrlPath::fromString("/foo/bar/baz/qux");
    auto urlPath = descendentPath.getTail();

    ASSERT_TRUE(urlPath.isValid());
    ASSERT_FALSE(urlPath.isEmpty());
    ASSERT_EQ (3, urlPath.numParts());
    ASSERT_EQ ("bar", urlPath.partView(0));
    ASSERT_EQ ("baz", urlPath.partView(1));
    ASSERT_EQ ("qux", urlPath.partView(2));
    ASSERT_EQ ("/bar/baz/qux", urlPath.pathView());
}

TEST(UrlPath, TestExtractTailSingleSegment)
{
    auto descendentPath = tempo_utils::UrlPath::fromString("/foo");
    auto urlPath = descendentPath.getTail();

    ASSERT_TRUE(urlPath.isValid());
    ASSERT_TRUE(urlPath.isEmpty());
    ASSERT_EQ (0, urlPath.numParts());
    ASSERT_EQ ("/", urlPath.pathView());
}

TEST(UrlPath, TestExtractTailEmpty)
{
    auto descendentPath = tempo_utils::UrlPath::fromString("/");
    auto urlPath = descendentPath.getTail();

    ASSERT_TRUE(urlPath.isValid());
    ASSERT_TRUE(urlPath.isEmpty());
    ASSERT_EQ (0, urlPath.numParts());
    ASSERT_EQ ("/", urlPath.pathView());
}

TEST(UrlPath, TestExtractTailInvalid)
{
    auto descendentPath = tempo_utils::UrlPath();
    auto urlPath = descendentPath.getTail();

    ASSERT_FALSE(urlPath.isValid());
    ASSERT_TRUE(urlPath.isEmpty());
    ASSERT_EQ (0, urlPath.numParts());
    ASSERT_EQ ("", urlPath.pathView());
}

TEST(UrlPath, TestIsParentOf)
{
    ASSERT_TRUE(tempo_utils::UrlPath::fromString("/")
        .isParentOf(tempo_utils::UrlPath::fromString("/foo")));
    ASSERT_TRUE(tempo_utils::UrlPath::fromString("/foo")
        .isParentOf(tempo_utils::UrlPath::fromString("/foo/bar")));
    ASSERT_TRUE(tempo_utils::UrlPath::fromString("/foo/bar/baz")
        .isParentOf(tempo_utils::UrlPath::fromString("/foo/bar/baz/qux")));

    ASSERT_FALSE(tempo_utils::UrlPath::fromString("/foo")
        .isParentOf(tempo_utils::UrlPath::fromString("/foo")));
    ASSERT_FALSE(tempo_utils::UrlPath::fromString("/foo")
        .isParentOf(tempo_utils::UrlPath::fromString("/")));
    ASSERT_FALSE(tempo_utils::UrlPath::fromString("/")
        .isParentOf(tempo_utils::UrlPath::fromString("/foo/bar")));
}

TEST(UrlPath, TestIsChildOf)
{
    ASSERT_TRUE(tempo_utils::UrlPath::fromString("/foo")
        .isChildOf(tempo_utils::UrlPath::fromString("/")));
    ASSERT_TRUE(tempo_utils::UrlPath::fromString("/foo/bar")
        .isChildOf(tempo_utils::UrlPath::fromString("/foo")));
    ASSERT_TRUE(tempo_utils::UrlPath::fromString("/foo/bar/baz/qux")
        .isChildOf(tempo_utils::UrlPath::fromString("/foo/bar/baz")));

    ASSERT_FALSE(tempo_utils::UrlPath::fromString("/foo")
        .isChildOf(tempo_utils::UrlPath::fromString("/foo")));
    ASSERT_FALSE(tempo_utils::UrlPath::fromString("/")
        .isChildOf(tempo_utils::UrlPath::fromString("/foo")));
    ASSERT_FALSE(tempo_utils::UrlPath::fromString("/foo/bar")
        .isChildOf(tempo_utils::UrlPath::fromString("/")));
}

TEST(UrlPath, TestIsAncestorOf)
{
    ASSERT_TRUE(tempo_utils::UrlPath::fromString("/")
        .isAncestorOf(tempo_utils::UrlPath::fromString("/foo")));
    ASSERT_TRUE(tempo_utils::UrlPath::fromString("/foo")
        .isAncestorOf(tempo_utils::UrlPath::fromString("/foo/bar")));
    ASSERT_TRUE(tempo_utils::UrlPath::fromString("/foo/bar/baz")
        .isAncestorOf(tempo_utils::UrlPath::fromString("/foo/bar/baz/qux")));
    ASSERT_TRUE(tempo_utils::UrlPath::fromString("/")
        .isAncestorOf(tempo_utils::UrlPath::fromString("/foo/bar")));
    ASSERT_TRUE(tempo_utils::UrlPath::fromString("/foo")
        .isAncestorOf(tempo_utils::UrlPath::fromString("/foo/bar/baz/qux")));

    ASSERT_FALSE(tempo_utils::UrlPath::fromString("/foo")
        .isAncestorOf(tempo_utils::UrlPath::fromString("/foo")));
    ASSERT_FALSE(tempo_utils::UrlPath::fromString("/foo")
        .isAncestorOf(tempo_utils::UrlPath::fromString("/")));
}

TEST(UrlPath, TestIsDescendentOf)
{
    ASSERT_TRUE(tempo_utils::UrlPath::fromString("/foo")
        .isDescendentOf(tempo_utils::UrlPath::fromString("/")));
    ASSERT_TRUE(tempo_utils::UrlPath::fromString("/foo/bar")
        .isDescendentOf(tempo_utils::UrlPath::fromString("/foo")));
    ASSERT_TRUE(tempo_utils::UrlPath::fromString("/foo/bar/baz/qux")
        .isDescendentOf(tempo_utils::UrlPath::fromString("/foo/bar/baz")));
    ASSERT_TRUE(tempo_utils::UrlPath::fromString("/foo/bar")
        .isDescendentOf(tempo_utils::UrlPath::fromString("/")));
    ASSERT_TRUE(tempo_utils::UrlPath::fromString("/foo/bar/baz/qux")
        .isDescendentOf(tempo_utils::UrlPath::fromString("/foo")));

    ASSERT_FALSE(tempo_utils::UrlPath::fromString("/foo")
        .isDescendentOf(tempo_utils::UrlPath::fromString("/foo")));
    ASSERT_FALSE(tempo_utils::UrlPath::fromString("/")
        .isDescendentOf(tempo_utils::UrlPath::fromString("/foo")));
    ASSERT_FALSE(tempo_utils::UrlPath::fromString("/")
        .isDescendentOf(tempo_utils::UrlPath::fromString("/foo/bar")));
}

TEST(UrlPath, TestTraverse)
{
    auto basePath = tempo_utils::UrlPath::fromString("/foo/bar");
    auto urlPath = basePath.traverse(tempo_utils::UrlPathPart("baz"));

    ASSERT_FALSE(urlPath.isEmpty());
    ASSERT_EQ (3, urlPath.numParts());
    ASSERT_EQ ("foo", urlPath.partView(0));
    ASSERT_EQ ("bar", urlPath.partView(1));
    ASSERT_EQ ("baz", urlPath.partView(2));
}

TEST(UrlPath, TestTraverseParentSegment)
{
    auto basePath = tempo_utils::UrlPath::fromString("/foo/bar");
    auto urlPath = basePath.traverse(tempo_utils::UrlPathPart::parent());

    ASSERT_FALSE(urlPath.isEmpty());
    ASSERT_EQ (2, urlPath.numParts());
    ASSERT_EQ ("foo", urlPath.partView(0));
    ASSERT_EQ ("", urlPath.partView(1));
}

TEST(UrlPath, TestTraverseMultipleSegments)
{
    auto basePath = tempo_utils::UrlPath::fromString("/foo/bar");
    auto urlPath = basePath.traverse(
        tempo_utils::UrlPathPart::parent(),
        tempo_utils::UrlPathPart("baz"),
        tempo_utils::UrlPathPart("qux"));

    ASSERT_FALSE(urlPath.isEmpty());
    ASSERT_EQ (3, urlPath.numParts());
    ASSERT_EQ ("foo", urlPath.partView(0));
    ASSERT_EQ ("baz", urlPath.partView(1));
    ASSERT_EQ ("qux", urlPath.partView(2));
}

TEST(UrlPath, TestTraversePath)
{
    auto basePath = tempo_utils::UrlPath::fromString("/foo/bar");
    auto urlPath = basePath.traverse(tempo_utils::UrlPath::fromString("baz"));

    ASSERT_FALSE(urlPath.isEmpty());
    ASSERT_EQ (3, urlPath.numParts());
    ASSERT_EQ ("foo", urlPath.partView(0));
    ASSERT_EQ ("bar", urlPath.partView(1));
    ASSERT_EQ ("baz", urlPath.partView(2));
}

TEST(UrlPath, TestTraversePathMultipleSegments)
{
    auto basePath = tempo_utils::UrlPath::fromString("/foo/bar");
    auto urlPath = basePath.traverse(tempo_utils::UrlPath::fromString("../baz/qux"));

    ASSERT_FALSE(urlPath.isEmpty());
    ASSERT_EQ (3, urlPath.numParts());
    ASSERT_EQ ("foo", urlPath.partView(0));
    ASSERT_EQ ("baz", urlPath.partView(1));
    ASSERT_EQ ("qux", urlPath.partView(2));
}

TEST(UrlPath, TestTraversePathsAndParts)
{
    auto basePath = tempo_utils::UrlPath::fromString("/");
    auto urlPath = basePath.traverse(
        tempo_utils::UrlPathPart("foo"),
        tempo_utils::UrlPathPart("bar"),
        tempo_utils::UrlPath::fromString("../baz/qux"));

    ASSERT_FALSE(urlPath.isEmpty());
    ASSERT_EQ (3, urlPath.numParts());
    ASSERT_EQ ("foo", urlPath.partView(0));
    ASSERT_EQ ("baz", urlPath.partView(1));
    ASSERT_EQ ("qux", urlPath.partView(2));
}

TEST(UrlPath, TestTraverseAbsolutePath)
{
    auto basePath = tempo_utils::UrlPath::fromString("/foo/bar");
    auto urlPath = basePath.traverse(tempo_utils::UrlPath::fromString("/baz"));

    ASSERT_FALSE(urlPath.isEmpty());
    ASSERT_EQ (1, urlPath.numParts());
    ASSERT_EQ ("baz", urlPath.partView(0));
}

TEST(UrlPath, TestConvertToFilesystemPathWithRootBaseDirectory)
{
    auto urlPath = tempo_utils::UrlPath::fromString("/foo/bar");
    auto rootdir = std::filesystem::current_path().root_directory();
    auto fsPath = urlPath.toFilesystemPath(rootdir);

    ASSERT_EQ ("/foo/bar", fsPath.string());
    ASSERT_TRUE (fsPath.has_root_directory());
    ASSERT_TRUE (fsPath.is_absolute());
}
TEST(UrlPath, TestConvertToFilesystemPathWithEmptyBaseDirectory)
{
    auto urlPath = tempo_utils::UrlPath::fromString("/foo/bar");
    auto fsPath = urlPath.toFilesystemPath({});

    ASSERT_EQ ("foo/bar", fsPath.string());
    ASSERT_FALSE (fsPath.has_root_directory());
    ASSERT_FALSE (fsPath.is_absolute());
}

TEST(UrlPath, TestConvertToFilesystemPathWithBaseDirectory)
{
    auto urlPath = tempo_utils::UrlPath::fromString("/foo/bar");
    auto cwd = std::filesystem::current_path();
    auto fsPath = urlPath.toFilesystemPath(cwd);

    ASSERT_EQ (cwd.string() + "/foo/bar", fsPath.string());
    ASSERT_TRUE (fsPath.has_root_directory());
    ASSERT_TRUE (fsPath.is_absolute());
}
