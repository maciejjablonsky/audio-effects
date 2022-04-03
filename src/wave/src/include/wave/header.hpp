#pragma once
#include <bit>
#include <cstdint>
#include <iterator>
#include <numeric>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/transform.hpp>
#include <string_view>

namespace splt::wave
{
namespace detail
{
constexpr uint32_t tag_to_integer(const std::string_view tag)
{
    if constexpr (std::endian::native != std::endian::little)
    {
        throw std::exception("Not supported endiannes.");
    }
    auto view = ranges::views::reverse(tag);
    return std::accumulate(std::begin(view),
                           std::end(view),
                           0U,
                           [](auto sum, auto c) { return sum << 8 | c; });
}
} // namespace detail

namespace format
{
constexpr uint16_t PCM = 1;
}
struct header
{
    uint32_t chunk_id         = detail::tag_to_integer("RIFF");
    uint32_t file_size        = sizeof(header) - 2 * sizeof(uint32_t);
    uint32_t format           = detail::tag_to_integer("WAVE");
    uint32_t chunk_marker     = detail::tag_to_integer("fmt ");
    uint32_t data_format_size = 16;
    uint16_t audio_format     = format::PCM;
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t bytes_per_second;
    uint16_t bytes_per_sample;
    uint16_t bits_per_sample;
    uint32_t data_id   = detail::tag_to_integer("data");
    uint32_t data_size = 0;

    header() = default;
    header(uint16_t channels, uint32_t sample_rate, uint16_t bits_per_sample)
        : channels(channels), sample_rate(sample_rate),
          bits_per_sample(bits_per_sample)
    {
        bytes_per_sample = bits_per_sample / 8;
        bytes_per_second = sample_rate * channels * bytes_per_sample;
    }
};
static_assert(std::is_trivially_copyable_v<header>);

constexpr auto file_size_skipped =
    sizeof(header::chunk_id) + sizeof(header::file_size);

constexpr bool validate_header(const header& h) noexcept
{
    bool valid = true;
    valid &= (h.chunk_id == detail::tag_to_integer("RIFF"));
    valid &= (h.format == detail::tag_to_integer("WAVE"));
    valid &= (h.chunk_marker == detail::tag_to_integer("fmt "));
    valid &= (h.data_id == detail::tag_to_integer("data"));
    valid &= (h.bytes_per_sample == (h.channels * h.bits_per_sample) / 8);
    valid &= (h.bytes_per_second ==
              (h.channels * h.sample_rate * h.bits_per_sample) / 8);
    return valid;
}

} // namespace splt::wave