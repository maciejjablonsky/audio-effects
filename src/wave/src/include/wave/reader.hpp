#pragma once
#include <concepts>
#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <span>
#include <variant>
#include <wave/header.hpp>

namespace splt::wave
{
template <typename T>
concept Reader = requires(T t, const T const_t, int16_t int16)
{
    T{std::filesystem::path{}};
    //{
    //    t.read(std::span<int16_t>{})
    //    } -> std::convertible_to<size_t>; // returns std::min(span.size(),
    //                                      // acutally_read)
    //{
    //    t.read(std::span<int32_t>{})
    //    } -> std::convertible_to<size_t>; // returns std::min(span.size(),
    //                                      // acutally_read)
    {
        const_t.frames_left()
        } -> std::convertible_to<size_t>;
    {
        const_t.bytes_left()
        } -> std::convertible_to<size_t>;
};

class reader
{
  private:
    std::FILE* const _file;
    const wave::header _header;

    wave::header _read_header();

  public:
    reader(const std::filesystem::path& path);
    ~reader();
    template <typename T> size_t read(std::span<T> buffer);
    size_t frames_left() const;
    size_t bytes_left() const;
    template <typename T> bool samples_as() const;
    void reset();
    bool eof() const;
};

template <typename T> size_t reader::read(std::span<T> buffer)
{
    return std::fread(buffer.data(), sizeof(T), buffer.size(), _file);
}

template <typename T> bool reader::samples_as() const
{
    return std::same_as<T, int16_t>;
}
static_assert(Reader<reader>);
} // namespace splt::wave
