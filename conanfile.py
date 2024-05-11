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
    options = {'shared': [True, False], 'compiler.cppstd': ['17', '20'], 'build_type': ['Debug', 'Release']}
    default_options = {'shared': True, 'compiler.cppstd': '20', 'build_type': 'Debug'}

    exports_sources = (
        'CMakeLists.txt',
        'cmake/*',
        'tempo_allocator/*',
        'tempo_command/*',
        'tempo_config/*',
        'tempo_security/*',
        'tempo_test/*',
        'tempo_tools/*',
        'tempo_tracing/*',
        'tempo_utils/*',
        )

    requires = (
        # requirements from timbre
        'absl/20230802.1@timbre',
        'antlr/4.9.3@timbre',
        'boost/1.84.0@timbre',
        'croaring/1.1.5@timbre',
        'fmt/9.1.0@timbre',
        'flatbuffers/23.5.26@timbre',
        'gtest/1.14.0@timbre',
        'hdrhistogram/0.9.6@timbre',
        'icu/74.1@timbre',
        'jemalloc/5.3.0@timbre',
        'openssl/3.2.0@timbre',
        'rapidjson/20230820.1@timbre',
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
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.install()

    def package(self):
        copy(self, "*.h", join(self.build_folder,'include'), join(self.package_folder,'include'))
        copy(self, "*.dll", join(self.build_folder,'bin'), join(self.package_folder,'bin'), keep_path=False)
        copy(self, "*.so", join(self.build_folder,'lib'), join(self.package_folder,'lib'), keep_path=False)
        copy(self, "*.dylib", join(self.build_folder,'lib'), join(self.package_folder,'lib'), keep_path=False)
        copy(self, "*.a", join(self.build_folder,'lib'), join(self.package_folder,'lib'), keep_path=False)
        copy(self, "*", join(self.build_folder,'lib','cmake'), join(self.package_folder,'lib','cmake'))

    def package_info(self):
        self.cpp_info.set_property("cmake_find_mode", "none")
        self.cpp_info.builddirs.append(join("lib", "cmake", "tempo"))
