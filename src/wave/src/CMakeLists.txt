find_package(fmt REQUIRED)

add_library(splt_wave reader.cpp)
add_library(splt::wave ALIAS splt_wave)
target_include_directories(splt_wave PUBLIC include)
target_link_libraries(splt_wave PRIVATE fmt::fmt)