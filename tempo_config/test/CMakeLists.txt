enable_testing()

include(GoogleTest)

# define unit tests

set(TEST_CASES
    base_conversions_tests.cpp
    config_listener_tests.cpp
    config_serde_tests.cpp
    container_conversions_tests.cpp
    extend_map_tests.cpp
    merge_map_tests.cpp
    program_config_tests.cpp
    workspace_config_tests.cpp
    )

# define test suite driver

add_executable(tempo_config_testsuite ${TEST_CASES})
target_link_libraries(tempo_config_testsuite PUBLIC
    Tempo::TempoConfig
    gtest::gtest
    )
gtest_discover_tests(tempo_config_testsuite)

# define test suite static library

add_library(TempoConfigTestSuite OBJECT ${TEST_CASES})
target_link_libraries(TempoConfigTestSuite PUBLIC
    Tempo::TempoConfig
    gtest::gtest
    )
