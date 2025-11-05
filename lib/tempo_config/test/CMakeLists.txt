enable_testing()

include(GoogleTest)

# define unit tests

set(TEST_CASES
    base_conversions_tests.cpp
    config_builder_tests.cpp
    config_compare_tests.cpp
    config_hash_tests.cpp
    config_listener_error_handling_tests.cpp
    config_listener_tests.cpp
    config_serde_tests.cpp
    container_conversions_tests.cpp
    extend_map_tests.cpp
    merge_map_tests.cpp
    number_conversions_tests.cpp
    program_config_tests.cpp
    time_conversion_tests.cpp
    workspace_config_tests.cpp
    )

# define test suite driver

add_executable(tempo_config_testsuite ${TEST_CASES} ${TEST_MAIN_CPP})
target_link_libraries(tempo_config_testsuite PUBLIC
    tempo::tempo_config
    tempo::tempo_test
    gtest::gtest
    )
gtest_discover_tests(tempo_config_testsuite)

# define test suite static library

add_library(TempoConfigTestSuite OBJECT ${TEST_CASES})
target_link_libraries(TempoConfigTestSuite PUBLIC
    tempo::tempo_config
    tempo::tempo_test
    gtest::gtest
    )
