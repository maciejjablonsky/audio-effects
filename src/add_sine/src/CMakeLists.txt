find_package(fmt REQUIRED)
find_package(cxxopts REQUIRED)

add_executable(splt_add_sine main.cpp)
target_link_libraries(splt_add_sine PRIVATE splt_wave fmt::fmt cxxopts::cxxopts)