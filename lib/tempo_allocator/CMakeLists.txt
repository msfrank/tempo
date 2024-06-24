
# do not run moc automatically
set(CMAKE_AUTOMOC OFF)

# build tempo_allocator as a shared library
add_library(tempo_allocator SHARED)
add_library(tempo::tempo_allocator ALIAS tempo_allocator)

set(TEMPO_ALLOCATOR_INCLUDES
    include/tempo_allocator/allocator_types.h
    include/tempo_allocator/arena_pool.h
    include/tempo_allocator/memory_allocator.h
    include/tempo_allocator/memory_pool.h
    include/tempo_allocator/tempo_allocator.h
    )
set_target_properties(tempo_allocator PROPERTIES PUBLIC_HEADER "${TEMPO_ALLOCATOR_INCLUDES}")

target_sources(tempo_allocator
    PRIVATE
    src/allocator_types.cpp
    src/arena_pool.cpp
    src/memory_allocator.cpp
    src/memory_pool.cpp
    )

# set the library version
set_target_properties(tempo_allocator PROPERTIES VERSION "${FULL_VERSION}" SOVERSION "${MAJOR_VERSION}")

# set the RPATH if on OS X
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(tempo_allocator PROPERTIES MACOSX_RPATH TRUE)
endif()

set_target_properties(tempo_allocator PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${TEMPO_BUILD_LIB_DIR}
    INSTALL_RPATH_USE_LINK_PATH TRUE
    INSTALL_RPATH ${LIB_RPATH}
    )

# set the public header include path differently on the target depending on the interface
target_include_directories(tempo_allocator PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

target_link_libraries(tempo_allocator
    PUBLIC
    tempo::tempo_utils
    absl::flat_hash_map
    fmt::fmt
    PRIVATE
    absl::strings
    jemalloc::jemalloc
    )

# install targets
install(TARGETS tempo_allocator EXPORT tempo-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tempo_allocator
    )

# add testing subdirectory
add_subdirectory(test)
