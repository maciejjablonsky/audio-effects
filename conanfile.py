from conans import ConanFile, CMake, tools


class SPLTConanFile(ConanFile):
    name = 'splt'
    settings = 'os', 'compiler', 'arch', 'build_type'
    generators = 'cmake_paths','cmake_find_package'
    requires = 'fmt/8.1.1', 'gtest/cci.20210126', 'range-v3/0.11.0', 'cxxopts/3.0.0'

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
