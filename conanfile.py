from os.path import join

from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy

class Tempo(ConanFile):
    name = 'tempo'
    version = '0.0.1'
    license = 'BSD-3-Clause'
    url = 'https://github.com/msfrank/tempo'
    description = 'Utility libraries for the Zuri project'

    settings = 'os', 'compiler', 'build_type', 'arch'
    options = {
        'use_sanitizer': [True, False, None],
        'sanitizer': ['address', 'thread', 'memory', 'ub', 'leak', None],
        'use_profiler': [True, False, None],
        'docker_program': ['ANY', None],
        'docker_requires_sudo': [True, False, None],
        'docker_platform_id': ['ANY', None],
        'docker_registry': ['ANY', None],
    }
    default_options = {
        'use_sanitizer': None,
        'sanitizer': None,
        'use_profiler': None,
        'docker_program': None,
        'docker_requires_sudo': False,
        'docker_platform_id': None,
        'docker_registry': None,
    }

    exports_sources = (
        'CMakeLists.txt',
        'bin/*',
        'cmake/*',
        'lib/*',
        )

    requires = (
        # requirements from timbre
        'absl/20250127.1@timbre',
        'antlr/4.9.3@timbre',
        'boost/1.88.0@timbre',
        'croaring/1.1.5@timbre',
        'fmt/9.1.0@timbre',
        'flatbuffers/23.5.26@timbre',
        'gtest/1.14.0@timbre',
        'hdrhistogram/0.9.6@timbre',
        'jemalloc/5.3.0@timbre',
        'openssl/3.2.0@timbre',
        'rapidjson/20250205.1@timbre',
        'utfcpp/4.0.6@timbre',
        )

    def validate(self):
        check_min_cppstd(self, "20")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        antlr = self.dependencies['antlr'].buildenv_info.vars(self)
        flatbuffers = self.dependencies['flatbuffers'].buildenv_info.vars(self)

        tc = CMakeToolchain(self)
        tc.cache_variables['TEMPO_PACKAGE_VERSION'] = self.version
        tc.cache_variables['TEMPO_PACKAGE_URL'] = self.url
        tc.cache_variables['TEMPO_PACKAGE_DESCRIPTION'] = self.description
        tc.cache_variables['TEMPO_PACKAGE_LICENSE'] = self.license
        tc.cache_variables['ANTLR_TOOL_JAR'] = antlr.get('ANTLR_TOOL_JAR')
        tc.cache_variables['FLATBUFFERS_FLATC'] = flatbuffers.get('FLATBUFFERS_FLATC')

        if self.options.use_sanitizer:
            tc.cache_variables['USE_SANITIZER'] = self.options.use_sanitizer
        if self.options.sanitizer:
            tc.cache_variables['SANITIZER'] = self.options.sanitizer
        if self.options.use_profiler:
            tc.cache_variables['USE_PROFILER'] = self.options.use_profiler
        if self.options.docker_program:
            tc.cache_variables['DOCKER_PROGRAM'] = self.options.docker_program
        if self.options.docker_requires_sudo is not None:
            tc.cache_variables['DOCKER_REQUIRES_SUDO'] = self.options.docker_requires_sudo
        if self.options.docker_platform_id:
            tc.cache_variables['DOCKER_PLATFORM_ID'] = self.options.docker_platform_id
        if self.options.docker_registry:
            tc.cache_variables['DOCKER_REGISTRY'] = self.options.docker_registry

        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.set_property("cmake_find_mode", "none")
        self.cpp_info.builddirs.append(join("lib", "cmake", "tempo"))
