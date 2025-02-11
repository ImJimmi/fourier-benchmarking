#pragma once

#include <complex>
#include <vector>

[[nodiscard]] std::vector<std::complex<float>> metalDFT(const std::vector<float>& frequencies,
                                                        float sampleRate,
                                                        const std::vector<std::complex<float>>& input);
