#include "benchmarking.h"
#include "dft.h"
#include "fft.h"
#include "math.h"

#include <complex>
#include <juce_core/juce_core.h>

void testFFT(int order)
{
    const auto size = 1 << (order);
    const auto input = randomComplexBuffer(size * 2);

    const auto durationSeconds = 3.0;
    const auto result = executeBenchmark(juce::RelativeTime::seconds(durationSeconds), [&] {
                            const auto output = fft(order, input);
                        })
                      / durationSeconds;

    const auto sampleRate = 44100.0;
    const auto minFreq = sampleRate / static_cast<double>(size);
    std::cout << "| " << size << " | " << result << " | " << minFreq << "Hz |\n";
}

void testDFT(int size)
{
    const auto sampleRate = 44100.0f;
    const auto frequencies = logRange(20.0f, 20000.0f, size);
    const auto input = randomComplexBuffer(sampleRate / 60.0f);

    const auto durationSeconds = 3.0;
    const auto result = executeBenchmark(juce::RelativeTime::seconds(durationSeconds), [&] {
                            const auto output = dft(frequencies, sampleRate, input);
                        })
                      / durationSeconds;

    std::cout << "| " << size << " | " << result << " | " << frequencies[1] - frequencies[0] << "Hz |\n";
}

int main()
{
    std::cout << "### FFT\n\n";
    std::cout << "| Buffer size | Iterations/second | Frequency Spacing (@44.1kHz) |\n";
    std::cout << "|-------------|-------------------|------------------------------|\n";

    for (auto i = 1; i < 16; i++)
        testFFT(i);

    std::cout << "\n";
    std::cout << "### DFT\n\n";
    std::cout << "| Buffer size | Iterations/second | Frequency Spacing (@44.1kHz) |\n";
    std::cout << "|-------------|-------------------|------------------------------|\n";

    for (auto i = 1; i < 16; i++)
        testDFT(1 << i);

    return 0;
}
