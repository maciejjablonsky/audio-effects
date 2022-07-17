#include <cassert>
#include <cxxopts.hpp>
#include <filesystem>
#include <fmt/format.h>
#include <numbers>
#include <oscillator/sine.hpp>
#include <wave/reader.hpp>
#include <wave/writer.hpp>

auto configure_cli()
{
    cxxopts::Options options(
        "add_sine", "takes wave file and adds sine wave of given frequency");
    auto options_adder = options.add_options();
    options_adder("i,input",
                  "path to input wave file",
                  cxxopts::value<std::filesystem::path>());
    options_adder("f,frequency",
                  "frequency of applied sine wave",
                  cxxopts::value<float>()->default_value("440"));
    options_adder("o,output",
                  "path to output wave file",
                  cxxopts::value<std::filesystem::path>());
    return options;
}

void apply_sine(const std::filesystem::path& input_file,
                const std::filesystem::path& output_file,
                float sound_frequency)
{
    splt::wave::reader input{input_file};
    assert(input.samples_as<int16_t>());
    auto h = input.header();
    splt::wave::writer output{h, output_file};

    splt::oscillator::sine g{sound_frequency, h.sample_rate};

    while (!input.eof())
    {
        constexpr auto samples_n = 1024;
        std::pmr::vector<int16_t> buffer(samples_n);
        auto read= input.read(std::span{buffer});
        std::for_each(std::begin(read), std::end(read), [&](auto& sample) {
            sample += 2000 * g.next_sample();
        });
        output.write(read);
    }
}

int main(const int argc, const char* argv[])
{
    auto cli    = configure_cli();
    auto result = cli.parse(argc, argv);

    const auto input_path = [&result] {
        if (!result["input"].count())
        {
            throw cxxopts::missing_argument_exception(
                "Path to input file missing.");
        }
        return result["input"].as<std::filesystem::path>();
    }();
    const auto output_path = [&result] {
        if (!result["output"].count())
        {
            throw cxxopts::missing_argument_exception(
                "Path to output file missing.");
        }
        return result["output"].as<std::filesystem::path>();
    }();
    const auto frequency = result["frequency"].as<float>();

    apply_sine(input_path, output_path, frequency);
}