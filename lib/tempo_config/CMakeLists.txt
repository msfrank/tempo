
# do not run moc automatically
set(CMAKE_AUTOMOC OFF)

# build tempo_config as a shared library
add_library(tempo_config SHARED)
add_library(tempo::tempo_config ALIAS tempo_config)

set(TEMPO_CONFIG_INCLUDES
    include/tempo_config/abstract_config_parser.h
    include/tempo_config/base_conversions.h
    include/tempo_config/config_result.h
    include/tempo_config/config_serde.h
    include/tempo_config/config_types.h
    include/tempo_config/container_conversions.h
    include/tempo_config/enum_conversions.h
    include/tempo_config/extend_map.h
    include/tempo_config/merge_map.h
    include/tempo_config/parse_config.h
    include/tempo_config/program_config.h
    include/tempo_config/workspace_config.h
    )
set_target_properties(tempo_config PROPERTIES PUBLIC_HEADER "${TEMPO_CONFIG_INCLUDES}")

# generate antlr4 files for Tempo config grammar
add_custom_command (
    OUTPUT
      ${CMAKE_CURRENT_BINARY_DIR}/src/ConfigLexer.cpp
      ${CMAKE_CURRENT_BINARY_DIR}/src/ConfigLexer.h
      ${CMAKE_CURRENT_BINARY_DIR}/src/ConfigLexer.interp
      ${CMAKE_CURRENT_BINARY_DIR}/src/ConfigLexer.tokens
      ${CMAKE_CURRENT_BINARY_DIR}/src/ConfigParserBaseListener.cpp
      ${CMAKE_CURRENT_BINARY_DIR}/src/ConfigParserBaseListener.h
      ${CMAKE_CURRENT_BINARY_DIR}/src/ConfigParser.cpp
      ${CMAKE_CURRENT_BINARY_DIR}/src/ConfigParser.h
      ${CMAKE_CURRENT_BINARY_DIR}/src/ConfigParser.interp
      ${CMAKE_CURRENT_BINARY_DIR}/src/ConfigParser.tokens
      ${CMAKE_CURRENT_BINARY_DIR}/src/ConfigParserListener.cpp
      ${CMAKE_CURRENT_BINARY_DIR}/src/ConfigParserListener.h
    COMMAND
      cmake -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/src
    COMMAND
      ${ANTLR}
      -o ${CMAKE_CURRENT_BINARY_DIR}/src
      -lib ${CMAKE_CURRENT_SOURCE_DIR}/share
      -Werror
      -Dlanguage=Cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/share/ConfigLexer.g4 ${CMAKE_CURRENT_SOURCE_DIR}/share/ConfigParser.g4
    DEPENDS
      ${CMAKE_CURRENT_SOURCE_DIR}/share/ConfigLexer.g4 ${CMAKE_CURRENT_SOURCE_DIR}/share/ConfigParser.g4
)

target_sources(tempo_config
    PRIVATE
    src/base_conversions.cpp
    src/config_result.cpp
    src/config_serde.cpp
    src/config_types.cpp
    src/extend_map.cpp
    src/merge_map.cpp
    src/parse_directories.cpp
    src/program_config.cpp
    src/workspace_config.cpp

    include/tempo_config/internal/abstract_json_node.h
    include/tempo_config/internal/config_listener.h
    src/internal/config_listener.cpp
    include/tempo_config/internal/json_array.h
    src/internal/json_array.cpp
    include/tempo_config/internal/json_object.h
    src/internal/json_object.cpp
    include/tempo_config/internal/json_value.h
    src/internal/json_value.cpp
    include/tempo_config/internal/lexer_error_listener.h
    src/internal/lexer_error_listener.cpp
    include/tempo_config/internal/parser_error_listener.h
    src/internal/parser_error_listener.cpp

    # antlr auto-generated sources
    ${CMAKE_CURRENT_BINARY_DIR}/src/ConfigLexer.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/src/ConfigLexer.h
    ${CMAKE_CURRENT_BINARY_DIR}/src/ConfigParserBaseListener.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/src/ConfigParserBaseListener.h
    ${CMAKE_CURRENT_BINARY_DIR}/src/ConfigParser.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/src/ConfigParser.h
    ${CMAKE_CURRENT_BINARY_DIR}/src/ConfigParserListener.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/src/ConfigParserListener.h
    )

# set the library version
set_target_properties(tempo_config PROPERTIES VERSION "${FULL_VERSION}" SOVERSION "${MAJOR_VERSION}")

# set the RPATH if on OS X
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(tempo_config PROPERTIES MACOSX_RPATH TRUE)
endif()

set_target_properties(tempo_config PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${TEMPO_BUILD_LIB_DIR}
    INSTALL_RPATH_USE_LINK_PATH TRUE
    INSTALL_RPATH ${LIB_RPATH}
    )

# set the public header include path differently on the target depending on the interface
target_include_directories(tempo_config PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

# include private headers
target_include_directories(tempo_config PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)

target_link_libraries(tempo_config
    PUBLIC
    tempo::tempo_utils
    absl::flat_hash_map
    absl::strings
    absl::time
    absl::time_zone
    fmt::fmt
    PRIVATE
    antlr::antlr
    icu::icu
    rapidjson::rapidjson
    )

# install targets
install(TARGETS tempo_config EXPORT tempo-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tempo_config
    )

# add testing subdirectory
add_subdirectory(test)
