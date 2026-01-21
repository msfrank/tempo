#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tempo_config/internal/piece_printer.h>
#include <tempo_config/internal/piece_store.h>
#include <tempo_test/tempo_test.h>

class PiecePrinter : public ::testing::Test {};

TEST_F(PiecePrinter, ParseInputAndPrintOutput)
{
    tempo_config::internal::PieceStore store;

    auto input = std::string(R"({"foo": /* comment */ "1"})");
    ASSERT_THAT (store.parse(input), tempo_test::IsOk());
    auto *root = store.root();

    tempo_config::internal::PrinterOptions options;
    std::string output;
    ASSERT_THAT (tempo_config::internal::print_root(options, root, output), tempo_test::IsOk());

    TU_CONSOLE_OUT << output;
    ASSERT_EQ (input, output);
}

TEST_F(PiecePrinter, ParseMultiLineInputAndPrintOutput)
{
    tempo_config::internal::PieceStore store;

    auto input = std::string(R"(
{
    "foo": /* comment */ "1"
}
)");

    ASSERT_THAT (store.parse(input), tempo_test::IsOk());
    auto *root = store.root();

    tempo_config::internal::PrinterOptions options;
    std::string output;
    ASSERT_THAT (tempo_config::internal::print_root(options, root, output), tempo_test::IsOk());

    TU_CONSOLE_OUT << output;
    ASSERT_EQ (input, output);
}