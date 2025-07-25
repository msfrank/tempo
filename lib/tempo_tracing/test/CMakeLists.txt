enable_testing()

include(GoogleTest)

# define unit tests

set(TEST_CASES
    current_scope_tests.cpp
    enter_scope_tests.cpp
    exit_scope_tests.cpp
    leaf_scope_tests.cpp
    multi_threading_tests.cpp
    span_failure_propagation_tests.cpp
    span_log_tests.cpp
    span_status_tests.cpp
    span_timing_tests.cpp
    trace_context_tests.cpp
    trace_span_tests.cpp
    unowned_trace_context_tests.cpp
    )

# define test suite driver

add_executable(tempo_tracing_testsuite ${TEST_CASES} base_tracing_fixture.cpp base_tracing_fixture.h)
target_include_directories(tempo_tracing_testsuite PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_BINARY_DIR}/../src
    )
target_link_libraries(tempo_tracing_testsuite PUBLIC
    tempo::tempo_test
    tempo::tempo_tracing
    flatbuffers::flatbuffers
    gtest::gtest
    )
gtest_discover_tests(tempo_tracing_testsuite)

# define test suite static library

add_library(TempoTracingTestSuite OBJECT ${TEST_CASES} base_tracing_fixture.cpp base_tracing_fixture.h)
target_include_directories(TempoTracingTestSuite PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_BINARY_DIR}/../src
)
target_link_libraries(TempoTracingTestSuite PUBLIC
    tempo::tempo_test
    tempo::tempo_tracing
    flatbuffers::flatbuffers
    gtest::gtest
    )
