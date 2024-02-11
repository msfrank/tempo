enable_testing()

include(GoogleTest)

# define unit tests

set(TEST_CASES
    command_parser_tests.cpp
    command_tokenizer_tests.cpp
)

# define test suite driver

add_executable(tempo_command_testsuite ${TEST_CASES})
target_link_libraries(tempo_command_testsuite PUBLIC
    Tempo::TempoCommand
    gtest::gtest
)
gtest_discover_tests(tempo_command_testsuite)

# define test suite static library

add_library(TempoCommandTestSuite OBJECT ${TEST_CASES})
target_link_libraries(TempoCommandTestSuite PUBLIC
    Tempo::TempoCommand
    gtest::gtest
)
