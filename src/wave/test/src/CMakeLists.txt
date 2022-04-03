find_package(GTest REQUIRED)
find_package(fmt REQUIRED)

add_executable(wave_test main.cpp wave_reader_test.cpp wave_writer_test.cpp) 
target_link_libraries(wave_test splt::wave GTest::gtest fmt::fmt)

target_compile_definitions(wave_test PRIVATE -DSPLT_SAMPLES_DIRECTORY="${SPLT_SAMPLES_DIR}")

# make tmp folder and create c++ define with path to tmp folder
set(TMP_DIR ${CMAKE_CURRENT_BINARY_DIR}/tmp)
file(MAKE_DIRECTORY ${TMP_DIR})
target_compile_definitions(wave_test PRIVATE -DSPLT_TMP_DIRECTORY="${TMP_DIR}")
