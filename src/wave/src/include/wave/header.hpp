#pragma once
#include <cstdint>

namespace splt::wave
{
struct header
{
    uint32_t chunk_id;
    uint32_t chunk_size;
    uint32_t format;
    uint32_t chunk_marker;
    uint32_t data_format_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t bytes_per_second;
    uint16_t bytes_per_sample;
    uint16_t bits_per_sample;
    uint32_t data_id;
    uint32_t data_size;
};
} // namespace splt::wave