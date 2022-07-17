#include <gtest/gtest.h>
#include <span>
#include <vector>
#include <wave/reader.hpp>

TEST(WaveReaderTest, Read)
{
    splt::wave::reader reader(std::filesystem::path{SPLT_SAMPLES_DIRECTORY} /
                              "input" /
                              "chameleon_01_44100Hz_16bit_1ch_100bpm.wav");
    EXPECT_EQ(reader.bytes_left(),
              0x1D11E0); // read only header, so all bytes are left
    EXPECT_EQ(reader.frames_left(),
              0xE88F0); // read only header, so all frames are left
    EXPECT_TRUE(reader.samples_as<int16_t>());
    std::pmr::vector<int16_t> samples_buffer{reader.frames_left(),
                                             std::pmr::get_default_resource()};
    std::span samples_span{samples_buffer.data(), samples_buffer.size()};
    auto samples_read = reader.read(samples_span);
    EXPECT_EQ(samples_read.size(), 0xE88F0);
}

TEST(WaveReaderTest, BulkAndStreamingRead)
{
    splt::wave::reader bulk_reader(
        std::filesystem::path{SPLT_SAMPLES_DIRECTORY} / "input" /
        "chameleon_01_44100Hz_16bit_1ch_100bpm.wav");
    std::pmr::vector<int16_t> bulk_samples_buffer{
        bulk_reader.frames_left(), std::pmr::get_default_resource()};
    std::span bulk_samples_span{bulk_samples_buffer.data(),
                                bulk_samples_buffer.size()};
    auto bulk_samples_read = bulk_reader.read(bulk_samples_span);
    bool eof_triggered     = [&] { // try to read just one sample to trigger EOF
        int16_t dummy_sample = 0;
        std::span dummy_span{std::addressof(dummy_sample), 1};
        auto samples_read = bulk_reader.read(dummy_span);
        return bulk_reader.eof();
    }();
    ASSERT_TRUE(eof_triggered);

    splt::wave::reader streaming_reader(
        std::filesystem::path{SPLT_SAMPLES_DIRECTORY} / "input" /
        "chameleon_01_44100Hz_16bit_1ch_100bpm.wav");
    std::pmr::vector<int16_t> streaming_samples_buffer{
        streaming_reader.frames_left(), std::pmr::get_default_resource()};
    for (auto* write_position = streaming_samples_buffer.data();
         !streaming_reader.eof();)
    {
        constexpr auto chunk_size = 0x400;
        std::span read_span{write_position, chunk_size};
        auto read_count = streaming_reader.read(read_span);
        write_position += read_count.size();
    }
    EXPECT_EQ(bulk_samples_buffer, streaming_samples_buffer);
    ASSERT_TRUE(bulk_reader.eof());
}