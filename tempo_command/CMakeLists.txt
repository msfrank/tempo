
# do not run moc automatically
set(CMAKE_AUTOMOC OFF)

# build TempoCommand as a shared library
add_library(TempoCommand SHARED)
add_library(Tempo::TempoCommand ALIAS TempoCommand)

set(TEMPO_COMMAND_INCLUDES
    include/tempo_command/command_config.h
    include/tempo_command/command_help.h
    include/tempo_command/command_parser.h
    include/tempo_command/command_result.h
    include/tempo_command/command_tokenizer.h
    include/tempo_command/tempo_command.h
    )
set_target_properties(TempoCommand PROPERTIES PUBLIC_HEADER "${TEMPO_COMMAND_INCLUDES}")

target_sources(TempoCommand
    PRIVATE
    src/command_config.cpp
    src/command_help.cpp
    src/command_parser.cpp
    src/command_result.cpp
    src/command_tokenizer.cpp
    )

# set the library version
set_target_properties(TempoCommand PROPERTIES VERSION "${FULL_VERSION}" SOVERSION "${MAJOR_VERSION}")

# set the RPATH if on OS X
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(TempoCommand PROPERTIES MACOSX_RPATH TRUE)
endif()

set_target_properties(TempoCommand PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

# set the public header include path differently on the target depending on the interface
target_include_directories(TempoCommand PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

target_link_libraries(TempoCommand
    PUBLIC
    Tempo::TempoConfig
    Tempo::TempoUtils
    absl::flat_hash_map
    absl::strings
    )

# install targets
install(TARGETS TempoCommand EXPORT Tempo-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tempo_command
    )

# add testing subdirectory
add_subdirectory(test)
