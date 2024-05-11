
# do not run moc automatically
set(CMAKE_AUTOMOC OFF)

# build tempo_security as a shared library
add_library(tempo_security SHARED)
add_library(tempo::tempo_security ALIAS tempo_security)

set(TEMPO_SECURITY_INCLUDES
    include/tempo_security/abstract_private_key_generator.h
    include/tempo_security/certificate_key_pair.h
    include/tempo_security/csr_key_pair.h
    include/tempo_security/ecc_key.h
    include/tempo_security/ecc_private_key_generator.h
    include/tempo_security/generate_utils.h
    include/tempo_security/rsa_key.h
    include/tempo_security/rsa_private_key_generator.h
    include/tempo_security/security_result.h
    include/tempo_security/security_types.h
    include/tempo_security/tempo_security.h
    include/tempo_security/x509_certificate.h
    include/tempo_security/x509_certificate_signing_request.h
    include/tempo_security/x509_store.h
    )
set_target_properties(tempo_security PROPERTIES PUBLIC_HEADER "${TEMPO_SECURITY_INCLUDES}")

target_sources(tempo_security
    PRIVATE
    src/certificate_key_pair.cpp
    src/csr_key_pair.cpp
    src/ecc_key.cpp
    src/ecc_private_key_generator.cpp
    src/generate_utils.cpp
    src/rsa_key.cpp
    src/rsa_private_key_generator.cpp
    src/security_result.cpp
    src/security_types.cpp
    src/x509_certificate.cpp
    src/x509_certificate_signing_request.cpp
    src/x509_store.cpp
)

# set the library version
set_target_properties(tempo_security PROPERTIES VERSION "${FULL_VERSION}" SOVERSION "${MAJOR_VERSION}")

# set the RPATH if on OS X
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(tempo_security PROPERTIES MACOSX_RPATH TRUE)
endif()

set_target_properties(tempo_security PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

# set the public header include path differently on the target depending on the interface
target_include_directories(tempo_security PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

target_link_libraries(tempo_security
    PUBLIC
    tempo::tempo_utils
    absl::flat_hash_map
    openssl::openssl
    )

# install targets
install(TARGETS tempo_security EXPORT tempo-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tempo_security
    )

# add testing subdirectory
add_subdirectory(test)
