
# do not run moc automatically
set(CMAKE_AUTOMOC OFF)

# build TempoAllocator as a shared library
add_library(TempoAllocator SHARED)
add_library(Tempo::TempoAllocator ALIAS TempoAllocator)

set(TEMPO_ALLOCATOR_INCLUDES
    include/tempo_allocator/allocator_types.h
    include/tempo_allocator/arena_pool.h
    include/tempo_allocator/memory_allocator.h
    include/tempo_allocator/memory_pool.h
    include/tempo_allocator/tempo_allocator.h
    )
set_target_properties(TempoAllocator PROPERTIES PUBLIC_HEADER "${TEMPO_ALLOCATOR_INCLUDES}")

target_sources(TempoAllocator
    PRIVATE
    src/allocator_types.cpp
    src/arena_pool.cpp
    src/memory_allocator.cpp
    src/memory_pool.cpp
    )

# set the library version
set_target_properties(TempoAllocator PROPERTIES VERSION "${FULL_VERSION}" SOVERSION "${MAJOR_VERSION}")

# set the RPATH if on OS X
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(TempoAllocator PROPERTIES MACOSX_RPATH TRUE)
endif()

set_target_properties(TempoAllocator PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

# set the public header include path differently on the target depending on the interface
target_include_directories(TempoAllocator PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

# make jemalloc headers visible internally
target_include_directories(TempoAllocator PRIVATE ${JEMALLOC_INCLUDE_DIRS})

# add jemalloc lib directory to the linker search path
target_link_directories(TempoAllocator PRIVATE ${JEMALLOC_LIBRARY_DIRS})

target_link_libraries(TempoAllocator
    PUBLIC
    Tempo::TempoUtils
    absl::flat_hash_map
    absl::strings
    fmt::fmt
    PRIVATE
    jemalloc::jemalloc
    )

# install targets
install(TARGETS TempoAllocator EXPORT Tempo-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tempo_allocator
    )

# add testing subdirectory
add_subdirectory(test)
