
# do not run moc automatically
set(CMAKE_AUTOMOC OFF)

# build tempo_utils as a shared library
add_library(tempo_utils SHARED)
add_library(tempo::tempo_utils ALIAS tempo_utils)

set(TEMPO_UTILS_INCLUDES
    include/tempo_utils/attr.h
    include/tempo_utils/attr_result.h
    include/tempo_utils/bytes_appender.h
    include/tempo_utils/compressed_bitmap.h
    include/tempo_utils/daemon_process.h
    include/tempo_utils/date_time.h
    include/tempo_utils/directory_maker.h
    include/tempo_utils/either_template.h
    include/tempo_utils/big_endian.h
    include/tempo_utils/fault_handler.h
    include/tempo_utils/file_appender.h
    include/tempo_utils/file_reader.h
    include/tempo_utils/file_result.h
    include/tempo_utils/file_utilities.h
    include/tempo_utils/file_writer.h
    include/tempo_utils/fp_compensation.h
    include/tempo_utils/hdr_histogram.h
    include/tempo_utils/ieee754.h
    include/tempo_utils/immutable_bytes.h
    include/tempo_utils/integer_types.h
    include/tempo_utils/iterator_template.h
    include/tempo_utils/library_loader.h
    include/tempo_utils/library_result.h
    include/tempo_utils/logging.h
    include/tempo_utils/log_console.h
    include/tempo_utils/log_helpers.h
    include/tempo_utils/log_message.h
    include/tempo_utils/log_stream.h
    include/tempo_utils/memory_bytes.h
    include/tempo_utils/memory_mapped_bytes.h
    include/tempo_utils/option_template.h
    include/tempo_utils/path_template.h
    include/tempo_utils/posix_result.h
    include/tempo_utils/prehashed_value.h
    include/tempo_utils/process_utils.h
    include/tempo_utils/result.h
    include/tempo_utils/schema.h
    include/tempo_utils/slice.h
    include/tempo_utils/status.h
    include/tempo_utils/tempdir_maker.h
    include/tempo_utils/tempfile_maker.h
    include/tempo_utils/tempo_utils.h
    include/tempo_utils/tracing.h
    include/tempo_utils/unicode.h
    include/tempo_utils/url.h
    include/tempo_utils/url_authority.h
    include/tempo_utils/url_origin.h
    include/tempo_utils/url_path.h
    include/tempo_utils/url_serde.h
    include/tempo_utils/url_types.h
    include/tempo_utils/user_home.h
    include/tempo_utils/uuid.h
    include/tempo_utils/vocabulary_cache.h
    )
set_target_properties(tempo_utils PROPERTIES PUBLIC_HEADER "${TEMPO_UTILS_INCLUDES}")

target_sources(tempo_utils
    PRIVATE
    src/attr.cpp
    src/attr_result.cpp
    src/bytes_appender.cpp
    src/compressed_bitmap.cpp
    src/daemon_process.cpp
    src/date_time.cpp
    src/directory_maker.cpp
    src/big_endian.cpp
    src/fault_handler.cpp
    src/file_appender.cpp
    src/file_reader.cpp
    src/file_result.cpp
    src/file_utilities.cpp
    src/file_writer.cpp
    src/fp_compensation.cpp
    src/hdr_histogram.cpp
    src/ieee754.cpp
    src/library_loader.cpp
    src/library_result.cpp
    src/logging.cpp
    src/log_console.cpp
    src/log_helpers.cpp
    src/log_message.cpp
    src/log_stream.cpp
    src/memory_bytes.cpp
    src/memory_mapped_bytes.cpp
    src/posix_result.cpp
    src/process_utils.cpp
    src/result.cpp
    src/slice.cpp
    src/status.cpp
    src/tempdir_maker.cpp
    src/tempfile_maker.cpp
    src/tracing.cpp
    src/unicode.cpp
    src/url.cpp
    src/url_authority.cpp
    src/url_origin.cpp
    src/url_path.cpp
    src/url_serde.cpp
    src/user_home.cpp
    src/uuid.cpp
    src/vocabulary_cache.cpp

    include/tempo_utils/internal/url_data.h
    src/internal/url_data.cpp
)

# set the library version
set_target_properties(tempo_utils PROPERTIES VERSION "${FULL_VERSION}" SOVERSION "${MAJOR_VERSION}")

# set the RPATH if on OS X
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(tempo_utils PROPERTIES MACOSX_RPATH TRUE)
endif()

set_target_properties(tempo_utils PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

# set the public header include path differently on the target depending on the interface
target_include_directories(tempo_utils PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

target_link_libraries(tempo_utils
    PUBLIC
    absl::flat_hash_map
    absl::strings
    absl::time
    absl::time_zone
    boost::headers
    fmt::fmt
    icu::icu
    PRIVATE
    ada::ada
    croaring::croaring
    hdrhistogram::hdrhistogram
    )

# install targets
install(TARGETS tempo_utils EXPORT tempo-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tempo_utils
    )

# add testing subdirectory
add_subdirectory(test)
