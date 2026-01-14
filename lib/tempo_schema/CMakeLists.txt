
# do not run moc automatically
set(CMAKE_AUTOMOC OFF)

# build tempo_schema as a shared library
add_library(tempo_schema SHARED)
add_library(tempo::tempo_schema ALIAS tempo_schema)

# add library to list of devkit targets
list(APPEND TEMPO_DEVKIT_TARGETS "tempo_schema")
set(TEMPO_DEVKIT_TARGETS ${TEMPO_DEVKIT_TARGETS} CACHE INTERNAL "")

set(TEMPO_SCHEMA_INCLUDES
    include/tempo_schema/abstract_attr_parser.h
    include/tempo_schema/abstract_attr_writer.h
    include/tempo_schema/abstract_attr_validator.h
    include/tempo_schema/attr.h
    include/tempo_schema/attr_serde.h
    include/tempo_schema/dcmi_elements_schema.h
    include/tempo_schema/path_serde.h
    include/tempo_schema/schema.h
    include/tempo_schema/schema_identifier.h
    include/tempo_schema/schema_namespace.h
    include/tempo_schema/schema_resource.h
    include/tempo_schema/schema_result.h
    include/tempo_schema/stateful_serde.h
    include/tempo_schema/url_serde.h
    include/tempo_schema/vocabulary_cache.h
    )
set_target_properties(tempo_schema PROPERTIES PUBLIC_HEADER "${TEMPO_SCHEMA_INCLUDES}")

target_sources(tempo_schema
    PRIVATE
    src/attr.cpp
    src/attr_serde.cpp
    src/path_serde.cpp
    src/schema_result.cpp
    src/url_serde.cpp
    src/vocabulary_cache.cpp
)

# set the library version
set_target_properties(tempo_schema PROPERTIES VERSION "${PROJECT_VERSION}" SOVERSION "${PROJECT_VERSION_MAJOR}")

# set the RPATH if on OS X
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(tempo_schema PROPERTIES MACOSX_RPATH TRUE)
endif()

set_target_properties(tempo_schema PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${TEMPO_BUILD_LIB_DIR}
    INSTALL_RPATH_USE_LINK_PATH TRUE
    INSTALL_RPATH ${LIB_RPATH}
    )

# set the public header include path differently on the target depending on the interface
target_include_directories(tempo_schema PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

target_link_libraries(tempo_schema
    PUBLIC
    tempo::tempo_utils
    PRIVATE
    Boost::headers
    )

# install targets
install(TARGETS tempo_schema EXPORT tempo-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tempo_schema
    )

# add testing subdirectory
add_subdirectory(test)
