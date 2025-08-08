
# define unit tests

set(TEST_CASES
    spanset_matcher_tests.cpp
    status_matcher_tests.cpp
    result_matcher_tests.cpp
    )

# define test suite driver

add_executable(tempo_test_testsuite ${TEST_CASES} ${TEST_MAIN_CPP})
target_link_libraries(tempo_test_testsuite PUBLIC
    tempo::tempo_test
    tempo::tempo_tracing
    gtest::gtest
    )
gtest_discover_tests(tempo_test_testsuite)

# define test suite static library

add_library(TempoTestTestSuite OBJECT ${TEST_CASES})
target_link_libraries(TempoTestTestSuite PUBLIC
    tempo::tempo_test
    tempo::tempo_tracing
    gtest::gtest
    )
