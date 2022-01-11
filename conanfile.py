from conans import ConanFile, CMake, tools


class AudioEffectsConanFile(ConanFile):
    name = 'audio-effects'
    settings = 'os', 'compiler', 'arch', 'build_type'
    generators = 'cmake_paths', 'cmake_find_package'
    requires = 'fmt/8.1.1', 'gtest/cci.20210126'

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()