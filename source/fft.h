#pragma once

#include <juce_dsp/juce_dsp.h>

[[nodiscard]] inline auto fft(int order, const std::vector<std::complex<float>>& input)
{
    juce::dsp::FFT fft{ order };
    std::vector<std::complex<float>> output(std::size(input));
    fft.perform(std::data(input), std::data(output), false);
    return output;
}
