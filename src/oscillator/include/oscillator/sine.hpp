#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>
#include <numbers>

namespace splt::oscillator
{
class sine
{
  private:
    const float _wave_frequency;
    const uint32_t _sampling_frequency;
    int64_t _sample_index = 0;

  public:
    inline sine(float wave_frequency, uint32_t sampling_frequency)
        : _wave_frequency(wave_frequency),
          _sampling_frequency(sampling_frequency),
          _sample_index(-wave_frequency)
    {
        assert(0 != _wave_frequency);
        assert(0 != _sampling_frequency);
    }

    inline float next_sample()
    {
        _sample_index = std::round(_sample_index + _wave_frequency);
        _sample_index %= _sampling_frequency;

        return std::sin(
            2 * std::numbers::pi *
            (static_cast<double>(_sample_index) / _sampling_frequency));
    }

    inline void reset()
    {
        _sample_index = 0;
    }
};
} // namespace splt::oscillator