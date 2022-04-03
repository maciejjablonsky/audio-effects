#include <algorithm>
#include <array>
#include <cstdio>
#include <filesystem>
#include <fmt/format.h>
#include <gtest/gtest.h>
#include <tuple>
#include <vector>
#include <wave/reader.hpp>
#include <wave/writer.hpp>

std::pmr::vector<std::byte> read_file(const std::filesystem::path& path)
{
    std::FILE* file = std::fopen(path.string().c_str(), "rb");
    if (!file)
    {
        throw std::runtime_error(
            fmt::format("Could not open file {}", path.string()));
    }
    const auto file_size = std::filesystem::file_size(path);
    std::pmr::vector<std::byte> buffer(file_size);
    auto read = std::fread(buffer.data(), sizeof(std::byte), file_size, file);
    assert(read == file_size);
    return buffer;
}

TEST(WaveWriterTest, Write)
{
    constexpr auto samples = [] {
        std::array<int16_t, 16> samples;
        std::iota(samples.begin(), samples.end(), 0);
        return samples;
    }();
    splt::wave::header h{1, 44100, 16};
    auto output_path =
        std::filesystem::path(SPLT_TMP_DIRECTORY) / "write_test.wav";
    splt::wave::writer writer{h, output_path};
    std::span samples_span{samples.data(), samples.size()};
    auto written = writer.write(samples_span);
    EXPECT_EQ(written, samples.size());
}

TEST(WaveWriterTest, ReaderReads_WriterWrites)
{
    splt::wave::reader reader(std::filesystem::path{SPLT_SAMPLES_DIRECTORY} /
                              "input" /
                              "chameleon_01_44100Hz_16bit_1ch_100bpm.wav");
    auto header        = reader.header();
    const auto samples = [&] {
        assert(1 == header.channels);
        std::pmr::vector<int16_t> samples{reader.frames_left(),
                                          std::pmr::get_default_resource()};
        std::span samples_span{samples.data(), samples.size()};
        reader.read(samples_span);
        return samples;
    }();

    header.file_size = 0;
    header.data_size = 0;
    {
        splt::wave::writer writer{header,
                                  std::filesystem::path{SPLT_TMP_DIRECTORY} /
                                      "reader_writer_test.wav"};
        for (auto *write_position = samples.data(),
                  *end            = samples.data() + samples.size();
             write_position < end;
             write_position += writer.write(std::span{
                 write_position,
                 std::min(static_cast<size_t>(0x400),
                          static_cast<size_t>(end - write_position))}))
        {
        }
    }

    auto original =
        read_file(std::filesystem::path{SPLT_SAMPLES_DIRECTORY} / "input" /
                  "chameleon_01_44100Hz_16bit_1ch_100bpm.wav");
    auto written = read_file(std::filesystem::path{SPLT_TMP_DIRECTORY} /
                             "reader_writer_test.wav");
    EXPECT_EQ(original, written);
}
