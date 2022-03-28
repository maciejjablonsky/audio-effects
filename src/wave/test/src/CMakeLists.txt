find_package(GTest REQUIRED)

add_executable(wave_test main.cpp wave_reader_test.cpp) 
target_link_libraries(wave_test splt::wave GTest::gtest)

target_compile_definitions(wave_test PRIVATE -DSPLT_SAMPLES_DIRECTORY="${SPLT_SAMPLES_DIR}")