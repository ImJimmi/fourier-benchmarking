#include "benchmarking.h"
#include "dft.h"
#include "fft.h"
#include "hlsl-dft.h"
#include "math.h"
#include "metal-dft.h"

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
    const auto input = randomComplexBuffer(size * 2);

    const auto durationSeconds = 3.0;
    const auto result = executeBenchmark(juce::RelativeTime::seconds(durationSeconds), [&] {
                            const auto output = dft(frequencies, sampleRate, input);
                        })
                      / durationSeconds;

    std::cout << "| " << size << " | " << result << " | " << frequencies[1] - frequencies[0] << "Hz |\n";
}

#if JUCE_MAC
void testMetalDFT(int size)
{
    const auto sampleRate = 44100.0f;
    const auto frequencies = logRange(20.0f, 20000.0f, size);
    const auto input = randomComplexBuffer(size * 2);

    const auto durationSeconds = 3.0;
    const auto result = executeBenchmark(juce::RelativeTime::seconds(durationSeconds), [&] {
                            const auto output = metalDFT(frequencies, sampleRate, input);
                        })
                      / durationSeconds;

    std::cout << "| " << size << " | " << result << " | " << frequencies[1] - frequencies[0] << "Hz |\n";
}
#endif

#if JUCE_WINDOWS
void testHLSLDFT(int size)
{
    const auto sampleRate = 44100.0f;
    const auto frequencies = logRange(20.0f, 20000.0f, size);
    const auto input = randomComplexBuffer(size * 2);

    const auto durationSeconds = 3.0;
    const auto result = executeBenchmark(juce::RelativeTime::seconds(durationSeconds), [&] {
                            const auto output = hlslDFT(frequencies, sampleRate, input);
                        })
                      / durationSeconds;

    std::cout << "| " << size << " | " << result << " | " << frequencies[1] - frequencies[0] << "Hz |\n";
}
#endif

int main()
{
    std::cout << "### FFT\n\n";
    std::cout << "| Buffer size | Iterations/second | Frequency Spacing (@44.1kHz) |\n";
    std::cout << "|-------------|-------------------|------------------------------|\n";

    for (auto i = 5; i < 12; i++)
        testFFT(i);

    std::cout << "\n";
    std::cout << "### DFT\n\n";
    std::cout << "| Buffer size | Iterations/second | Frequency Spacing (@44.1kHz) |\n";
    std::cout << "|-------------|-------------------|------------------------------|\n";

    for (auto i = 5; i < 14; i++)
        testDFT(1 << i);

#if JUCE_MAC
    std::cout << "\n";
    std::cout << "### Metal DFT\n\n";
    std::cout << "| Buffer size | Iterations/second | Frequency Spacing (@44.1kHz) |\n";
    std::cout << "|-------------|-------------------|------------------------------|\n";

    for (auto i = 5; i < 16; i++)
        testMetalDFT(1 << i);
#endif

#if JUCE_WINDOWS
    std::cout << "\n";
    std::cout << "### HLSL DFT\n\n";
    std::cout << "| Buffer size | Iterations/second | Frequency Spacing (@44.1kHz) |\n";
    std::cout << "|-------------|-------------------|------------------------------|\n";

    for (auto i = 5; i < 16; i++)
        testHLSLDFT(1 << i);
#endif

    // const auto size = 1 << 5;
    // const auto sampleRate = 44100.0f;
    // const auto frequencies = logRange(20.0f, 20000.0f, size);
    // const auto input = randomComplexBuffer(size);

    // const auto cpuOut = dft(frequencies, sampleRate, input);
    // const auto gpuOut = metalDFT(frequencies, sampleRate, input);

    // for (auto i = 0; i < size; i++)
    // {
    //     std::cout << i << ": " << cpuOut[i] << " - " << gpuOut[i] << "\n";
    // }

    return 0;
}
