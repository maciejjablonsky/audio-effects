cmake_minimum_required(VERSION 3.17)
project(audio-effects)

set(USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(3rd_party)
add_subdirectory(src)
