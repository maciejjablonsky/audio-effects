find_package(fmt REQUIRED)
find_package(range-v3 REQUIRED)

add_library(splt_wave reader.cpp writer.cpp)
add_library(splt::wave ALIAS splt_wave)
target_include_directories(splt_wave PUBLIC include)
target_link_libraries(splt_wave PRIVATE fmt::fmt)
target_link_libraries(splt_wave PUBLIC range-v3::range-v3)