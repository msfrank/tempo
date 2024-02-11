
add_executable(tempo-bytes2code src/bytes_to_code.cpp)

set_target_properties(tempo-bytes2code PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

target_link_libraries(tempo-bytes2code
    PUBLIC
    Tempo::TempoCommand
    Tempo::TempoConfig
    Tempo::TempoUtils
    absl::strings
    absl::time
    absl::time_zone
    fmt::fmt
    flatbuffers::flatbuffers
    )

# install targets
install(
    TARGETS
      tempo-bytes2code
    EXPORT Tempo-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
