enable_testing()

include(GoogleTest)

# define unit tests

set(TEMPO_UTILS_TEST_CASES
    bytes_appender_tests.cpp
    big_endian_tests.cpp
    hdr_histogram_tests.cpp
    ieee754_tests.cpp
    logging_tests.cpp
    memory_bytes_tests.cpp
    memory_mapped_bytes_tests.cpp
    prehashed_value_tests.cpp
    slice_tests.cpp
    status_tests.cpp
    tempdir_maker_tests.cpp
    unicode_tests.cpp
    url_path_tests.cpp
    url_tests.cpp
    uuid_tests.cpp
    )

# define test suite driver

add_executable(tempo_utils_testsuite ${TEMPO_UTILS_TEST_CASES})
target_link_libraries(tempo_utils_testsuite PUBLIC
    tempo::tempo_utils
    gtest::gtest
    )
gtest_discover_tests(tempo_utils_testsuite)

# define test suite static library

add_library(TempoUtilsTestSuite OBJECT ${TEMPO_UTILS_TEST_CASES})
target_link_libraries(TempoUtilsTestSuite PUBLIC
    tempo::tempo_utils
    gtest::gtest
    )
