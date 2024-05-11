
# do not run moc automatically
set(CMAKE_AUTOMOC OFF)

# build tempo_tracing as a shared library
add_library(tempo_tracing SHARED)
add_library(tempo::tempo_tracing ALIAS tempo_tracing)

set(TEMPO_TRACING_INCLUDES
    include/tempo_tracing/active_scope.h
    include/tempo_tracing/enter_scope.h
    include/tempo_tracing/error_walker.h
    include/tempo_tracing/log_walker.h
    include/tempo_tracing/root_walker.h
    include/tempo_tracing/scope_manager.h
    include/tempo_tracing/span_log.h
    include/tempo_tracing/spanset_attr_parser.h
    include/tempo_tracing/spanset_attr_writer.h
    include/tempo_tracing/spanset_state.h
    include/tempo_tracing/span_walker.h
    include/tempo_tracing/tempo_spanset.h
    include/tempo_tracing/trace_recorder.h
    include/tempo_tracing/trace_span.h
    include/tempo_tracing/tracing_result.h
    include/tempo_tracing/tracing_schema.h
    include/tempo_tracing/tracing_types.h
    )
set_target_properties(tempo_tracing PROPERTIES PUBLIC_HEADER "${TEMPO_TRACING_INCLUDES}")

# generate flatbuffer files for spanset IDL
add_custom_command (
    OUTPUT
      ${CMAKE_CURRENT_BINARY_DIR}/include/tempo_tracing/generated/spanset.h
    COMMAND
      cmake -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/include/tempo_tracing/generated
    COMMAND
      ${FLATC} --cpp --scoped-enums --gen-mutable --gen-object-api --filename-suffix ''
      -o ${CMAKE_CURRENT_BINARY_DIR}/include/tempo_tracing/generated
      ${CMAKE_CURRENT_SOURCE_DIR}/share/spanset.fbs
    DEPENDS
      ${CMAKE_CURRENT_SOURCE_DIR}/share/spanset.fbs
)

# generate header containing the spanset schema
add_custom_command(
    OUTPUT
      ${CMAKE_CURRENT_BINARY_DIR}/include/tempo_tracing/generated/spanset_schema.h
    COMMAND
      cmake -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/include/tempo_tracing/generated
    COMMAND
      tempo-bytes2code
      -g TEMPO_TRACING_GENERATED_SPANSET_SCHEMA_H -n tempo_tracing::schema::spanset
      ${CMAKE_CURRENT_SOURCE_DIR}/share/spanset.fbs
      ${CMAKE_CURRENT_BINARY_DIR}/include/tempo_tracing/generated/spanset_schema.h
    DEPENDS
      ${CMAKE_CURRENT_SOURCE_DIR}/share/spanset.fbs
)

target_sources(tempo_tracing PRIVATE
    src/active_scope.cpp
    src/enter_scope.cpp
    src/error_walker.cpp
    src/log_walker.cpp
    src/root_walker.cpp
    src/scope_manager.cpp
    src/span_log.cpp
    src/spanset_attr_parser.cpp
    src/spanset_attr_writer.cpp
    src/spanset_state.cpp
    src/span_walker.cpp
    src/tempo_spanset.cpp
    src/trace_recorder.cpp
    src/trace_span.cpp
    src/tracing_result.cpp
    src/tracing_schema.cpp
    src/tracing_types.cpp

    include/tempo_tracing/internal/spanset_reader.h
    src/internal/spanset_reader.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/include/tempo_tracing/generated/spanset.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/tempo_tracing/generated/spanset_schema.h

    ${TEMPO_TRACING_INCLUDES}
    )

# set the library version
set_target_properties(tempo_tracing PROPERTIES VERSION "${FULL_VERSION}" SOVERSION "${MAJOR_VERSION}")

# set the RPATH if on OS X
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(tempo_tracing PROPERTIES MACOSX_RPATH TRUE)
endif()

set_target_properties(tempo_tracing PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

# set the public header include path differently on the target depending on the interface
target_include_directories(tempo_tracing PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

# make private headers visible internally
target_include_directories(tempo_tracing PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)

target_link_libraries(tempo_tracing
    PUBLIC
    tempo::tempo_utils
    absl::time
    fmt::fmt
    PRIVATE
    flatbuffers::flatbuffers
    )

# install targets
install(TARGETS tempo_tracing EXPORT tempo-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tempo_tracing
    )

# install data
install(FILES share/spanset.fbs DESTINATION ${CMAKE_INSTALL_DATADIR}/tempo_tracing)

# add testing subdirectory
add_subdirectory(test)
