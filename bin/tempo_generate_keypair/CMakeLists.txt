
add_executable(tempo-generate-keypair src/tempo_generate_keypair.cpp)

set_target_properties(tempo-generate-keypair PROPERTIES
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
