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
        'shared': [True, False],
        'compiler.cppstd': ['17', '20'],
        'build_type': ['Debug', 'Release'],
        'docker_program': ['ANY', None],
        'docker_requires_sudo': [True, False, None],
        'docker_platform_id': ['ANY', None],
        'docker_registry': ['ANY', None],
    }
    default_options = {
        'shared': True,
        'compiler.cppstd': '20',
        'build_type': 'Debug',
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
        tc.variables['TEMPO_PACKAGE_VERSION'] = self.version
        tc.variables['ANTLR_TOOL_JAR'] = antlr.get('ANTLR_TOOL_JAR')
        tc.variables['FLATBUFFERS_FLATC'] = flatbuffers.get('FLATBUFFERS_FLATC')

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
