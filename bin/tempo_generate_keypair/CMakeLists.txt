
add_executable(tempo-generate-keypair src/tempo_generate_keypair.cpp)

# set the binary version
set_target_properties(tempo-generate-keypair PROPERTIES VERSION "${PROJECT_VERSION}" SOVERSION "${PROJECT_VERSION_MAJOR}")

set_target_properties(tempo-generate-keypair PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${TEMPO_BUILD_BIN_DIR}
    INSTALL_RPATH_USE_LINK_PATH TRUE
    INSTALL_RPATH ${BIN_RPATH}
    )

target_link_libraries(tempo-generate-keypair
    PUBLIC
    tempo::tempo_command
    tempo::tempo_config
    tempo::tempo_security
    tempo::tempo_utils
    )

# install targets
install(TARGETS tempo-generate-keypair EXPORT tempo-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
