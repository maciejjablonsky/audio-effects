from conans import ConanFile, CMake, tools
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain
import os

class SPLTConanFile(ConanFile):
    name = 'splt'
    settings = 'os', 'compiler', 'arch', 'build_type'
    generators = 'CMakeDeps', 'CMakeToolchain'
    requires = 'fmt/8.1.1', 'gtest/cci.20210126', 'range-v3/0.11.0', 'cxxopts/3.0.0', 'boost/1.79.0'
    build_requires = 'cmake/[>= 3.21]'

    default_options = {
        'boost:header_only': True
    }

    def generate(self):
        tc = CMakeToolchain(self, generator='Ninja')
        tc.variables["SPLT_SAMPLES_DIR"] = os.path.join(self.recipe_folder, "samples").replace('\\', '/')
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()


    def build(self):
        cmake = CMake(self)
        if self.should_configure:
            cmake.configure()
        if self.should_build:
            cmake.build()
