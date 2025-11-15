enable_testing()

include(GoogleTest)

# define unit tests

set(TEST_CASES
    certificate_key_pair_tests.cpp
    csr_key_pair_tests.cpp
    digest_tests.cpp
    private_key_tests.cpp
    sha256_hash_tests.cpp
    x509_certificate_signing_request_tests.cpp
    x509_certificate_tests.cpp
    x509_store_tests.cpp
    )

# define test suite driver

add_executable(tempo_security_testsuite ${TEST_CASES} ${TEST_MAIN_CPP})
target_link_libraries(tempo_security_testsuite PUBLIC
    tempo::tempo_security
    OpenSSL::SSL
    OpenSSL::Crypto
    gtest::gtest
    )
gtest_discover_tests(tempo_security_testsuite)

# define test suite static library

add_library(TempoSecurityTestSuite OBJECT ${TEST_CASES})
target_link_libraries(TempoSecurityTestSuite PUBLIC
    tempo::tempo_security
    OpenSSL::SSL
    OpenSSL::Crypto
    gtest::gtest
    )
