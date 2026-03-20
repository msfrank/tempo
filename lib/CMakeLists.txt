
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

set(EMPTY_CPP ${CMAKE_CURRENT_BINARY_DIR}/empty.cpp)

file(TOUCH ${EMPTY_CPP})

add_executable(tempo_testsuite ${EMPTY_CPP})
target_link_libraries(tempo_testsuite
    PUBLIC
      tempo::tempo_test_main
      gtest::gtest
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

add_custom_target(run-full-testsuite
    COMMAND
      tempo_testsuite
        --gtest_output=xml:${CMAKE_BINARY_DIR}/full_testsuite_output.xml
        --logging-option severityFilter VeryVerbose
        --logging-option logSink LogFile
        --logging-option logFilePath ${CMAKE_BINARY_DIR}/full_testsuite.log
    )
