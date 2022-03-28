cmake_minimum_required(VERSION 3.17)
project(audio-effects)

set(USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

add_subdirectory(samples)
add_subdirectory(src)
message(${SPLT_SAMPLES_DIR})
