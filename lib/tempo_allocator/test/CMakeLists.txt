enable_testing()

include(GoogleTest)

# define unit tests

set(TEST_CASES
    arena_pool_tests.cpp
    memory_pool_tests.cpp
    )

# define test suite driver

add_executable(tempo_allocator_testsuite ${TEST_CASES} ${TEST_MAIN_CPP})
target_link_libraries(tempo_allocator_testsuite PUBLIC
    tempo::tempo_allocator
    gtest::gtest
    )
gtest_discover_tests(tempo_allocator_testsuite)

# define test suite static library

add_library(TempoAllocatorTestSuite OBJECT ${TEST_CASES})
target_link_libraries(TempoAllocatorTestSuite PUBLIC
    tempo::tempo_allocator
    gtest::gtest
    )
