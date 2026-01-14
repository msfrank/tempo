enable_testing()

include(GoogleTest)

# define unit tests

set(TEST_CASES
    attr_value_tests.cpp
    bool_attr_serde_tests.cpp
    path_serde_tests.cpp
    )

# define test suite driver

add_executable(tempo_schema_testsuite ${TEST_CASES} ${TEST_MAIN_CPP})
target_link_libraries(tempo_schema_testsuite PUBLIC
    tempo::tempo_schema
    gtest::gtest
    )
gtest_discover_tests(tempo_schema_testsuite)

# define test suite static library

add_library(TempoSchemaTestSuite OBJECT ${TEST_CASES})
add_dependencies(TempoSchemaTestSuite process-runner-child)
target_link_libraries(TempoSchemaTestSuite PUBLIC
    tempo::tempo_schema
    gtest::gtest
    )
