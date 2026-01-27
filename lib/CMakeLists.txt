
# define the test_main.cpp in a single place for all test suites
set(TEST_MAIN_CPP ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp)


#######################################################################
#
# add targets from subdirectories
#
#######################################################################

add_subdirectory(tempo_allocator)
add_subdirectory(tempo_command)
add_subdirectory(tempo_config)
add_subdirectory(tempo_schema)
add_subdirectory(tempo_security)
add_subdirectory(tempo_test)
add_subdirectory(tempo_tracing)
add_subdirectory(tempo_utils)


#######################################################################
#
# declare the package test suite target
#
#######################################################################

add_executable(tempo_testsuite ${TEST_MAIN_CPP})
target_link_libraries(tempo_testsuite
        PUBLIC gtest::gtest
        PRIVATE
        TempoAllocatorTestSuite
        TempoCommandTestSuite
        TempoConfigTestSuite
        TempoSchemaTestSuite
        TempoSecurityTestSuite
        TempoTestTestSuite
        TempoTracingTestSuite
        TempoUtilsTestSuite
)
gtest_discover_tests(tempo_testsuite)