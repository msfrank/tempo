
# build tempo_test as a shared library
add_library(tempo_test SHARED)
add_library(tempo::tempo_test ALIAS tempo_test)

set(TEMPO_TEST_INCLUDES
    include/tempo_test/result_matchers.h
    include/tempo_test/spanset_matchers.h
    include/tempo_test/status_matchers.h
    include/tempo_test/tempo_test.h
    )
set_target_properties(tempo_test PROPERTIES PUBLIC_HEADER "${TEMPO_TEST_INCLUDES}")

target_sources(tempo_test PRIVATE
    src/spanset_matchers.cpp
    src/status_matchers.cpp
    ${TEMPO_TEST_INCLUDES}
    )

# set the library version
set_target_properties(tempo_test PROPERTIES VERSION "${FULL_VERSION}" SOVERSION "${MAJOR_VERSION}")

# set the RPATH if on OS X
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(tempo_test PROPERTIES MACOSX_RPATH TRUE)
endif()

set_target_properties(tempo_test PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

# set the public header include path differently on the target depending on the interface
target_include_directories(tempo_test PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

target_link_libraries(tempo_test
    PUBLIC
    tempo::tempo_tracing
    tempo::tempo_utils
    gtest::gtest
    )

# install targets
install(TARGETS tempo_test EXPORT tempo-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tempo_test
    )

# add testing subdirectory
add_subdirectory(test)
