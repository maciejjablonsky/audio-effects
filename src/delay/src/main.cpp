#include <boost/circular_buffer.hpp>
#include <cassert>
#include <chrono>
#include <cxxopts.hpp>
#include <delay/delay.hpp>
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

auto apply_delay(std::convertible_to<std::filesystem::path> auto input_path,
               std::convertible_to<std::filesystem::path> auto output_path,
               std::convertible_to<std::chrono::milliseconds> auto offset)
{

    using sample_t = int16_t;
    splt::wave::reader input{input_path};
    assert(input.samples_as<sample_t>());
    auto input_header = input.header();
    splt::wave::writer output{input.header(), output_path};

    splt::effects::Delay delay;
    delay.set_delay(offset);
    delay.set_sample_rate(input_header.sample_rate);

    while (!input.eof())
    {
        std::pmr::vector<sample_t> samples_buffer(1024);
        auto read_samples = input.read(std::span{samples_buffer});
        delay.process(samples_buffer);
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

    apply_delay(cli_args["input"].as<std::filesystem::path>(),
              cli_args["output"].as<std::filesystem::path>(),
              std::chrono::milliseconds{cli_args["delay"].as<uint64_t>()});
}