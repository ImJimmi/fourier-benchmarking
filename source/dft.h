#pragma once

#include <complex>
#include <numbers>
#include <vector>

[[nodiscard]] inline auto dft(const std::vector<float>& frequencies,
                              float sampleRate,
                              const std::vector<std::complex<float>>& input)
{
    static constexpr std::complex j{ 0.0f, 1.0f };
    const auto frequencyScaler = j * 2.0f * std::numbers::pi_v<float> / sampleRate;

    std::vector<std::complex<float>> output(std::size(frequencies));

    for (std::size_t i = 0; i < std::size(frequencies); i++)
    {
        const auto z = std::exp(frequencyScaler * frequencies[i]);
        const auto zReciprocal = 1.0f / z;

        auto result = std::complex<float>{};
        auto zFactor = z;

        for (auto& x : input)
        {
            zFactor *= zReciprocal;
            result += x * zFactor;
        }

        output[i] = result;
    }

    return output;
}
