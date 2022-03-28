#include <gtest/gtest.h>
#include <wave/reader.hpp>

TEST(WaveReaderTest, Read)
{
    splt::wave::reader reader(std::filesystem::path{SPLT_SAMPLES_DIRECTORY} /
                              "input" /
                              "chameleon_01_44100Hz_16bit_1ch_100bpm.wav");
}