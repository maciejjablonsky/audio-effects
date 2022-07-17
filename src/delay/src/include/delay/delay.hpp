#pragma once

#include <algorithm>
#include <boost/circular_buffer.hpp>
#include <concepts>
#include <ratio>
#include <span>

template <typename original_ratio = std::ratio<1, 1>>
auto to_samples(std::chrono::milliseconds period, auto sample_rate)
{
    using r = std::ratio_divide<original_ratio, std::kilo>;
    return static_cast<size_t>(
        period.count() * (static_cast<double>(sample_rate * r::num) / r::den));
}

namespace splt::effects
{
class Delay
{
  public:
    using sample_type = int16_t;

  private:
    double sample_rate_ = 44.1;           // in kHz
    std::chrono::milliseconds offset_{0}; // no delay by default
    boost::circular_buffer<sample_type> delay_line_;

    double feedback_level_ = 0.6;
    double output_level_   = 1 / 12.f;

    inline void resize_delay_line_()
    {
        const auto delay_length     = to_samples<std::kilo>(offset_, sample_rate_);
        constexpr auto frame_length = 1024;
        delay_line_ =
            boost::circular_buffer<sample_type>{delay_length + frame_length};
        std::fill_n(std::back_inserter(delay_line_),
                    delay_length,
                    static_cast<sample_type>(0));
    }

  public:
    template <typename original_ratio = std::ratio<1, 1>>
    inline void set_sample_rate(auto sample_rate)
    {
        using r      = std::ratio_divide<original_ratio, std::kilo>::type;
        sample_rate_ = static_cast<double>(sample_rate * r::num) / r::den;
        resize_delay_line_();
    }

    inline void set_delay(std::chrono::milliseconds offset)
    {
        offset_ = offset;
        resize_delay_line_();
    }

    inline void process(std::span<sample_type> samples)
    {
        // append new input to delay buffer
        std::copy(std::begin(samples),
                  std::end(samples),
                  std::back_inserter(delay_line_));
        // combine new input with delay line
        std::transform(std::begin(samples),
                       std::end(samples),
                       std::begin(delay_line_),
                       std::begin(samples),
                       [this](auto origin, auto delayed) {
                           return static_cast<sample_type>(
                               (origin + delayed * feedback_level_) *
                               output_level_);
                       });
    }
};
} // namespace splt::effects
