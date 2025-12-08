
# do not run moc automatically
set(CMAKE_AUTOMOC OFF)

# build tempo_command as a shared library
add_library(tempo_command SHARED)
add_library(tempo::tempo_command ALIAS tempo_command)

set(TEMPO_COMMAND_INCLUDES
    include/tempo_command/command_config.h
    include/tempo_command/command_help.h
    include/tempo_command/command_parser.h
    include/tempo_command/command_result.h
    include/tempo_command/command_tokenizer.h
    include/tempo_command/tempo_command.h
    )
set_target_properties(tempo_command PROPERTIES PUBLIC_HEADER "${TEMPO_COMMAND_INCLUDES}")

target_sources(tempo_command
    PRIVATE
    src/command_config.cpp
    src/command_help.cpp
    src/command_parser.cpp
    src/command_result.cpp
    src/command_tokenizer.cpp
    )

# set the library version
set_target_properties(tempo_command PROPERTIES VERSION "${PROJECT_VERSION}" SOVERSION "${PROJECT_VERSION_MAJOR}")

# set the RPATH if on OS X
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(tempo_command PROPERTIES MACOSX_RPATH TRUE)
endif()

set_target_properties(tempo_command PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${TEMPO_BUILD_LIB_DIR}
    INSTALL_RPATH_USE_LINK_PATH TRUE
    INSTALL_RPATH ${LIB_RPATH}
    )

# set the public header include path differently on the target depending on the interface
target_include_directories(tempo_command PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

target_link_libraries(tempo_command
    PUBLIC
    tempo::tempo_config
    tempo::tempo_utils
    absl::flat_hash_map
    absl::strings
    )

# install targets
install(TARGETS tempo_command EXPORT tempo-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tempo_command
    )

# add testing subdirectory
add_subdirectory(test)
