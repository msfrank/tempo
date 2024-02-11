enable_testing()

include(GoogleTest)

# define unit tests

set(TEST_CASES
    active_scope_tests.cpp
    span_log_tests.cpp
    status_span_tests.cpp
    )

# define test suite driver

add_executable(tempo_tracing_testsuite ${TEST_CASES})
target_include_directories(tempo_tracing_testsuite PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_BINARY_DIR}/../src
    )
target_link_libraries(tempo_tracing_testsuite PUBLIC
    Tempo::TempoTracing
    flatbuffers::flatbuffers
    gtest::gtest
    )
gtest_discover_tests(tempo_tracing_testsuite)

# define test suite static library

add_library(TempoTracingTestSuite OBJECT ${TEST_CASES})
target_include_directories(TempoTracingTestSuite PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_BINARY_DIR}/../src
)
target_link_libraries(TempoTracingTestSuite PUBLIC
    Tempo::TempoTracing
    flatbuffers::flatbuffers
    gtest::gtest
    )
