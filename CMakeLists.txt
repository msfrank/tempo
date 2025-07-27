cmake_minimum_required(VERSION 3.27)

set(TEMPO_PACKAGE_VERSION "0.0.0"
    CACHE STRING
    "The project version. Defaults to 0.0.0")
set(TEMPO_PACKAGE_URL ""
    CACHE STRING
    "The project url. Defaults to ''")
set(TEMPO_PACKAGE_DESCRIPTION ""
    CACHE STRING
    "The project description. Defaults to ''")
set(TEMPO_PACKAGE_LICENSE ""
    CACHE STRING
    "The project license. Defaults to ''")


project(tempo
    VERSION ${TEMPO_PACKAGE_VERSION}
    HOMEPAGE_URL ${TEMPO_PACKAGE_URL}
    DESCRIPTION ${TEMPO_PACKAGE_DESCRIPTION}
    )

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "build type not specified so defaulting to Debug" FORCE)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(IS_DEBUG_BUILD ON)
else()
    set(IS_DEBUG_BUILD OFF)
endif()

#######################################################################
#
# declare options
#
#######################################################################

option(USE_SANITIZER "Enable sanitizer library" OFF)

set(SANITIZER ""
    CACHE STRING
    "one of 'address', 'thread', 'memory', 'ub', or 'leak'")

set(SANITIZER_IGNORELIST ""
    CACHE PATH
    "Instruct sanitizer to ignore symbols specified in ignorelist")

option(USE_PROFILER "Enable gperftools profiler" OFF)

set(DOCKER_PROGRAM ""
    CACHE FILEPATH
    "Path to the docker command. If empty then the program path is auto-detected. Defaults to ''")

set(DOCKER_REQUIRES_SUDO "FALSE"
    CACHE BOOL
    "TRUE if the docker command requires sudo privileges. Defaults to 'FALSE'")

set(DOCKER_PLATFORM ""
    CACHE STRING
    "Target platform for docker image. If empty then the platform is auto-detected. Defaults to ''")

set(DOCKER_BASE_IMAGE_REGISTRY ""
    CACHE STRING
    "The docker registry containing the base image. Defaults to ''")

set(DOCKER_REGISTRY ""
    CACHE STRING
    "The docker registry to export to. If empty then defaults to the local image store. Defaults to ''")

#######################################################################
#
# configure compiler and linker
#
#######################################################################

# configure common compiler flags
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Werror -Wno-deprecated-declarations")

# by default do not specify -O
set (OPTIMIZATION_LEVEL "")

# enable one of the sanitizer libraries if specified
if (USE_SANITIZER)
    if ("${SANITIZER}" STREQUAL "address")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls")
        set (OPTIMIZATION_LEVEL "-O1")
        message(STATUS "enabling address sanitizer library")
    elseif ("${SANITIZER}" STREQUAL "thread")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
        set (OPTIMIZATION_LEVEL "-O1")
        message(STATUS "enabling thread sanitizer library")
    elseif ("${SANITIZER}" STREQUAL "memory")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=memory")
        set (OPTIMIZATION_LEVEL "-O1")
        message(STATUS "enabling memory sanitizer library")
    elseif ("${SANITIZER}" STREQUAL "ub")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
        set (OPTIMIZATION_LEVEL "-O1")
        message(STATUS "enabling ub sanitizer library")
    elseif ("${SANITIZER}" STREQUAL "leak")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
        set (OPTIMIZATION_LEVEL "-O1")
        message(STATUS "enabling leak sanitizer library")
    else()
        message(WARNING "skipping unknown sanitizer ${SANITIZER}")
        set (SANITIZER "")
    endif ("${SANITIZER}" STREQUAL "address")
    # if a sanitizer is enabled and an ignorelist is specified, then add it to the cxx flags
    if (SANITIZER)
        if (SANITIZER_IGNORELIST)
            set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize-ignorelist=${SANITIZER_IGNORELIST}")
            message(STATUS "using sanitizer ignorelist ${SANITIZER_IGNORELIST}")
        endif (SANITIZER_IGNORELIST)
    endif (SANITIZER)
else()
    message(STATUS "sanitizer not enabled")
endif (USE_SANITIZER)

#
if (NOT OPTIMIZATION_LEVEL)
    set(OPTIMIZATION_LEVEL "-O0 -fno-eliminate-unused-debug-types")
endif()

# set the optimization level
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPTIMIZATION_LEVEL}")

# define the rpath for executable and library targets
if (APPLE)
    set(BIN_RPATH "@executable_path/../lib")
    set(LIB_RPATH "@loader_path")
elseif(WIN32)
    # do nothing
else()
    set(BIN_RPATH "$ORIGIN/../lib")
    set(LIB_RPATH "$ORIGIN/")
endif()

#######################################################################
#
# find required and optional dependencies
#
#######################################################################

# find required Abseil dependencies
find_package(absl REQUIRED)

# find required Boost dependencies
find_package(Boost REQUIRED COMPONENTS headers url)

# find required Antlr dependency
find_package(antlr REQUIRED)

# find required Flatbuffers dependency
find_package(flatbuffers REQUIRED)

# find required Openssl dependency
find_package(openssl REQUIRED)

# find required Utfcpp dependency
find_package(utfcpp REQUIRED)

# find required Fmt dependency
find_package(fmt REQUIRED)

# find required jemalloc dependency
find_package(jemalloc REQUIRED)

# find required Rapidjson dependency
find_package(rapidjson REQUIRED)

# find required Croaring dependency
find_package(croaring REQUIRED)

# find required Hdr Histogram dependency
find_package(hdrhistogram REQUIRED)

# find java build tool dependency for antlr
find_package(Java REQUIRED)

# find google test libraries
find_package(gtest REQUIRED)

# link to gperftools profiler library if specified
set (PROFILER_LIBRARIES "")
if (${USE_PROFILER})
    set (PROFILER_LIBRARIES "-lprofiler")
    message(STATUS "enabling gperftools profiler")
else()
    message(STATUS "gperftools profiler not enabled")
endif(${USE_PROFILER})

# find docker CLI
if (DOCKER_PROGRAM)
    set(Docker ${DOCKER_PROGRAM})
    message(STATUS "Using Docker: ${Docker} (specified by DOCKER_PROGRAM)")
else()
    find_program(Docker docker)
    if (Docker EQUAL "Docker-NOTFOUND")
        message(STATUS "Docker not found, skipping generation of docker targets")
    else()
        message(STATUS "Found Docker: ${Docker}")
    endif()
endif()

# include needed CMake features
include(CMakePackageConfigHelpers)
include(CTest)
include(GNUInstallDirs)
include(GoogleTest)

#######################################################################
#
# configure tools and paths
#
#######################################################################

# define path to flatc
set(FLATC ${FLATBUFFERS_FLATC})
message(STATUS "flatc command is ${FLATC}")

# define path to antlr tool
set(ANTLR
    ${Java_JAVA_EXECUTABLE}
    -Xmx500M
    -cp ${ANTLR_TOOL_JAR}
    org.antlr.v4.Tool
)
message(STATUS "antlr command is ${ANTLR}")

if (DOCKER_REQUIRES_SUDO)
    set(DOCKER "sudo;${Docker}")
else ()
    set(DOCKER ${Docker})
endif ()

#######################################################################
#
# define build constants
#
#######################################################################

set(FULL_PLATFORM "${CMAKE_SYSTEM_NAME}.${CMAKE_SYSTEM_VERSION}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_CXX_COMPILER_ID}")
message(STATUS "full platform is ${FULL_PLATFORM}")

# define individual version components and version string
string(REGEX REPLACE "([0-9]+)\\.[0-9]+\\.[0-9]+" "\\1"
    MAJOR_VERSION
    "${TEMPO_PACKAGE_VERSION}")
string(REGEX REPLACE "[0-9]+\\.([0-9]+)\\.[0-9]+" "\\1"
    MINOR_VERSION
    "${TEMPO_PACKAGE_VERSION}")
string(REGEX REPLACE "[0-9]+\\.[0-9]+\\.([0-9]+)" "\\1"
    PATCH_VERSION
    "${TEMPO_PACKAGE_VERSION}")
set(FULL_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
message(STATUS "version is ${FULL_VERSION}")

# build paths
set(TEMPO_BUILD_OUTPUT_ROOT         "${CMAKE_BINARY_DIR}/output")
set(TEMPO_BUILD_BIN_DIR             "${TEMPO_BUILD_OUTPUT_ROOT}/${CMAKE_INSTALL_BINDIR}")
set(TEMPO_BUILD_LIB_DIR             "${TEMPO_BUILD_OUTPUT_ROOT}/${CMAKE_INSTALL_LIBDIR}")

#######################################################################
#
# add targets from subdirectories
#
#######################################################################

add_subdirectory(bin)
add_subdirectory(lib)

#######################################################################
#
# declare the package test suite target
#
#######################################################################

set(EMPTY_CPP ${CMAKE_CURRENT_BINARY_DIR}/empty.cpp)
add_custom_command(
    OUTPUT ${EMPTY_CPP}
    COMMAND cmake -E touch ${EMPTY_CPP}
    )

add_executable(tempo_testsuite ${EMPTY_CPP})
target_link_libraries(tempo_testsuite
    PUBLIC gtest::gtest
    PRIVATE
    TempoAllocatorTestSuite
    TempoConfigTestSuite
    TempoSchemaTestSuite
    TempoSecurityTestSuite
    TempoTestTestSuite
    TempoTracingTestSuite
    TempoUtilsTestSuite
)
gtest_discover_tests(tempo_testsuite)

#######################################################################
#
# create cmake package config
#
#######################################################################

configure_package_config_file(
    cmake/tempo-config.cmake.in
    tempo-config.cmake
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tempo"
    PATH_VARS CMAKE_INSTALL_BINDIR CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_DATADIR
    )

write_basic_package_version_file(tempo-config-version.cmake
    VERSION "${FULL_VERSION}"
    COMPATIBILITY SameMajorVersion
    )

install(EXPORT tempo-targets
    COMPONENT Devel
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tempo"
    NAMESPACE tempo::
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/tempo-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/tempo-config-version.cmake"
    COMPONENT Devel
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tempo"
    )


#######################################################################
#
# build docker image
#
#######################################################################

if (Docker)

    if (DOCKER_REGISTRY)
        set(DOCKER_OUTPUT "type=registry,push=true")
        set(DOCKER_TAG_PREFIX "${DOCKER_REGISTRY}/")
    else()
        set(DOCKER_OUTPUT "type=docker")
        set(DOCKER_TAG_PREFIX "")
    endif()

    # set the PLATFORM_ID
    if (DOCKER_PLATFORM)
        set(PLATFORM_ID ${DOCKER_PLATFORM})
    else()
        set(PLATFORM_ID "linux/${CMAKE_SYSTEM_PROCESSOR}")
    endif()
    if (NOT PLATFORM_ID)
        message(FATAL_ERROR "failed to determine docker platform id")
    endif()

    set(DOCKERFILE_PATH "${CMAKE_SOURCE_DIR}/docker/Dockerfile")

    # build the tempo/tempo image
    add_custom_target(build_docker_image
        COMMAND ${DOCKER} build
          --platform ${PLATFORM_ID}
          --build-arg base_image_registry=${DOCKER_BASE_IMAGE_REGISTRY}
          -o ${DOCKER_OUTPUT}
          -t "${DOCKER_TAG_PREFIX}tempo/tempo"
          -t "${DOCKER_TAG_PREFIX}tempo/tempo:${CMAKE_PROJECT_VERSION}"
          -f ${DOCKERFILE_PATH}
          --label "org.opencontainers.image.version=${CMAKE_PROJECT_VERSION}"
          --label "org.opencontainers.image.source=${CMAKE_PROJECT_HOMEPAGE_URL}"
          --label "org.opencontainers.image.description=${CMAKE_PROJECT_DESCRIPTION}"
          --label "org.opencontainers.image.licenses=${TEMPO_PACKAGE_LICENSE}"
          .
        COMMENT "building tempo/tempo docker image"
        DEPENDS ${DOCKERFILE_PATH}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )

endif()
