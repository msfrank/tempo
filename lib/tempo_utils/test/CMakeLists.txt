enable_testing()

include(GoogleTest)

add_executable(process-runner-child process_runner_child.cpp)
target_link_libraries(process-runner-child tempo::tempo_utils)

# define unit tests

set(TEMPO_UTILS_TEST_CASES
    bytes_appender_tests.cpp
    bytes_iterator_tests.cpp
    big_endian_tests.cpp
    hdr_histogram_tests.cpp
    ieee754_tests.cpp
    logging_tests.cpp
    memory_bytes_tests.cpp
    memory_mapped_bytes_tests.cpp
    prehashed_value_tests.cpp
    process_runner_tests.cpp
    slice_tests.cpp
    status_tests.cpp
    tempdir_maker_tests.cpp
    unicode_tests.cpp
    url_authority_tests.cpp
    url_origin_tests.cpp
    url_path_tests.cpp
    url_resolve_tests.cpp
    url_tests.cpp
    uuid_tests.cpp
    )

# define test suite driver

add_executable(tempo_utils_testsuite ${TEMPO_UTILS_TEST_CASES})
target_link_libraries(tempo_utils_testsuite PUBLIC
    tempo::tempo_utils
    gtest::gtest
    utfcpp::utfcpp
    )
add_dependencies(tempo_utils_testsuite process-runner-child)
target_compile_definitions(tempo_utils_testsuite PRIVATE
    "PROCESS_RUNNER_CHILD_EXECUTABLE=\"${CMAKE_CURRENT_BINARY_DIR}/process-runner-child\""
    )
gtest_discover_tests(tempo_utils_testsuite)

# define test suite static library

add_library(TempoUtilsTestSuite OBJECT ${TEMPO_UTILS_TEST_CASES})
add_dependencies(TempoUtilsTestSuite process-runner-child)
target_compile_definitions(TempoUtilsTestSuite PRIVATE
    "PROCESS_RUNNER_CHILD_EXECUTABLE=\"${CMAKE_CURRENT_BINARY_DIR}/process-runner-child\""
    )
target_link_libraries(TempoUtilsTestSuite PUBLIC
    tempo::tempo_utils
    gtest::gtest
    utfcpp::utfcpp
    )
