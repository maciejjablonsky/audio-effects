#include <boost/circular_buffer.hpp>
#include <cassert>
#include <chrono>
#include <cxxopts.hpp>
#include <filesystem>
#include <oscillator/sine.hpp>
#include <ratio>
#include <wave/reader.hpp>
#include <wave/writer.hpp>

auto cli()
{
    cxxopts::Options options("delay",
                             "takes wave file, offsets and sum with original");
    auto options_adder = options.add_options();
    options_adder("i,input",
                  "path to input wave file",
                  cxxopts::value<std::filesystem::path>());
    options_adder("delay",
                  "number of miliseconds to offset",
                  cxxopts::value<uint64_t>()->default_value("0"));
    options_adder("o,output",
                  "path to output wave file",
                  cxxopts::value<std::filesystem::path>());
    return options;
}

auto to_kilohertz(std::convertible_to<long double> auto hertz)
{
    return (long double)hertz / (std::kilo::num / std::kilo::den);
}

auto to_samples(std::chrono::milliseconds period,
                auto sample_rate_kHz)
{
    return static_cast<size_t>(period.count() * sample_rate_kHz);
}

template <typename T> auto apply_delay(std::span<T> samples, auto& delay_line)
{
    // append new input to delay buffer
    std::copy(
        std::begin(samples), std::end(samples), std::back_inserter(delay_line));
    // combine new input with delay line
    std::transform(std::begin(samples),
                   std::end(samples),
                   std::begin(delay_line),
                   std::begin(samples),
                   [](auto origin, auto delayed) {
                       constexpr auto feedback_level = 0.6;
                       constexpr auto output_level   = 1 / 12.f;
                       return static_cast<T>(
                           (origin + delayed * feedback_level) * output_level);
                   });
}

auto delay_sum(std::convertible_to<std::filesystem::path> auto input_path,
               std::convertible_to<std::filesystem::path> auto output_path,
               std::convertible_to<std::chrono::milliseconds> auto offset)
{
    using sample_t = int16_t;
    splt::wave::reader input{input_path};
    assert(input.samples_as<sample_t>());
    auto input_header = input.header();
    splt::wave::writer output{input.header(), output_path};

    const auto delay_length =
        to_samples(offset, to_kilohertz(input_header.sample_rate));
    constexpr auto frame_length = 1024; // in samples
    const auto delay_line_width = delay_length + frame_length;

    boost::circular_buffer<sample_t> delay_line(delay_line_width);
    std::fill_n(
        std::back_inserter(delay_line), delay_length, static_cast<sample_t>(0));

    while (!input.eof())
    {
        std::pmr::vector<sample_t> samples_buffer(frame_length);
        auto read_samples = input.read(std::span{samples_buffer});

        apply_delay(read_samples, delay_line);

        output.write(std::span{read_samples});
    }
}

int main(const int argc, const char* argv[])
{
    auto options  = cli();
    auto cli_args = options.parse(argc, argv);

    assert(cli_args["input"].count());
    assert(cli_args["output"].count());
    assert(cli_args["delay"].count());

    delay_sum(cli_args["input"].as<std::filesystem::path>(),
              cli_args["output"].as<std::filesystem::path>(),
              std::chrono::milliseconds{cli_args["delay"].as<uint64_t>()});
}