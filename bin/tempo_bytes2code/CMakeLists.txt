
add_library(bytes2code_serializer STATIC
    src/bytes_to_code_serializer.cpp
    src/bytes_to_code_serializer.h
    )

target_include_directories(bytes2code_serializer PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    )
target_link_libraries(bytes2code_serializer
    PUBLIC
    tempo::tempo_utils
    absl::strings
    )

add_executable(tempo-bytes2code src/bytes_to_code.cpp)

set_target_properties(tempo-bytes2code PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${TEMPO_BUILD_BIN_DIR}
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
    PRIVATE
    bytes2code_serializer
    )

# install targets
install(TARGETS tempo-bytes2code EXPORT tempo-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
