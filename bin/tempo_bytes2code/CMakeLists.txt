
add_executable(tempo-bytes2code src/bytes_to_code.cpp)

set_target_properties(tempo-bytes2code PROPERTIES
    INSTALL_RPATH_USE_LINK_PATH TRUE
    INSTALL_RPATH ${BIN_RPATH}
    )

target_link_libraries(tempo-bytes2code
    PUBLIC
    tempo::tempo_command
    tempo::tempo_config
    tempo::tempo_utils
    absl::strings
    absl::time
    absl::time_zone
    fmt::fmt
    )

# install targets
install(TARGETS tempo-bytes2code EXPORT tempo-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
