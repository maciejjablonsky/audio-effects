#pragma once
#include <concepts>
#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <span>
#include <wave/header.hpp>

namespace splt::wave
{
template <typename T>
concept Writer = requires(T t)
{
    T(header{}, std::filesystem::path{});
    {
        t.write(std::span<int16_t>{})
        } -> std::convertible_to<size_t>; // returns std::min(span.size(),
                                          // acutally_written)
    //{
    //    t.write(std::span<int32_t>{})
    //    } -> std::convertible_to<size_t>; // returns std::min(span.size(),
    //                                      // acutally_written)
    {
        t.samples_as()
        } -> std::convertible_to<bool>;
};

class writer
{
  private:
    std::FILE* const _file;
    wave::header _header{};

    void _write_header();

  public:
    writer(const wave::header& header, const std::filesystem::path& file_path);
    ~writer();
    template <typename T> size_t write(std::span<T> buffer);
    template <typename T> bool samples_as() const;
};

template <typename T> bool writer::samples_as() const
{
    return std::same_as<T, int16_t>;
}

template <typename T> size_t writer::write(std::span<T> buffer)
{
    auto written = std::fwrite(buffer.data(), sizeof(T), buffer.size(), _file);
    _header.file_size = std::ftell(_file) - wave::file_size_skipped;
    _header.data_size = _header.file_size + wave::file_size_skipped - sizeof(wave::header);
    return written;
};

} // namespace splt::wave
