#pragma once

template <typename Arithmetic>
[[nodiscard]] constexpr inline auto ratio(Arithmetic a, Arithmetic b)
{
    if constexpr (std::is_integral<Arithmetic>())
        return static_cast<double>(a) / static_cast<double>(b);
    else
        return a / b;
}

template <typename Arithmetic, typename Proportion = double>
[[nodiscard]] inline auto logSpace(Arithmetic start,
                                   Arithmetic end,
                                   Proportion proportion)
{
    static_assert(std::is_arithmetic<Arithmetic>());
    jassert(start > static_cast<Arithmetic>(0));
    jassert(end >= start);
    jassert(proportion >= 0 && proportion <= 1);

    if constexpr (std::is_integral<Arithmetic>())
        return start * std::pow(ratio(end, start), proportion);
    else
        return start * static_cast<Arithmetic>(std::pow(ratio(end, start), proportion));
}

template <typename Arithmetic, typename Size>
[[nodiscard]] inline auto logRange(Arithmetic start, Arithmetic end, Size count)
{
    static_assert(std::is_arithmetic<Arithmetic>());
    jassert(start > static_cast<Arithmetic>(0));
    jassert(end >= start);
    jassert(count >= 2);

    std::vector<decltype(logSpace(start, end, 0.0))> result(static_cast<std::size_t>(count));

    for (Size i = 0; i < count; i++)
    {
        const auto proportion = i / static_cast<double>(count - 1);
        result[static_cast<std::size_t>(i)] = logSpace(start, end, proportion);
    }

    return result;
}

[[nodiscard]] inline auto randomComplexBuffer(std::size_t size)
{
    static auto& rng = juce::Random::getSystemRandom();

    std::vector<std::complex<float>> buffer(size);

    for (auto& x : buffer)
        x = std::complex{ rng.nextFloat(), rng.nextFloat() };

    return buffer;
}
