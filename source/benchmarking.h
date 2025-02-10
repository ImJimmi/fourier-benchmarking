#pragma once

#include <juce_core/juce_core.h>

namespace detail
{
    template <typename Callable>
    struct BenchmarkThread : juce::Thread
    {
        BenchmarkThread(Callable&& fn)
            : juce::Thread{ "Benchmark Thread" }
            , function{ std::move(fn) }
        {
        }

        void run() final
        {
            while (!threadShouldExit())
            {
                std::invoke(function);
                iterations++;
            }
        }

        Callable function;
        std::atomic<int> iterations = 0;
    };
} // namespace detail

template <typename Callable>
[[nodiscard]] inline auto executeBenchmark(juce::RelativeTime duration, Callable&& callable)
{
    detail::BenchmarkThread<Callable> thread{ std::move(callable) };

    const auto start = juce::Time::getCurrentTime();
    thread.startThread();

    for (juce::RelativeTime elapsed;
         elapsed < duration;
         elapsed = juce::Time::getCurrentTime() - start)
    {
    }

    const auto result = thread.iterations.load();
    thread.stopThread(100000);
    thread.waitForThreadToExit(200000);

    return result;
}
