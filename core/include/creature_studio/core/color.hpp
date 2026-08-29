#pragma once

#include <cstdint>

namespace creature_studio::core
{

struct Color
{
    std::uint8_t r{0};
    std::uint8_t g{0};
    std::uint8_t b{0};
    std::uint8_t a{255};

    constexpr Color() = default;

    constexpr Color(
        std::uint8_t red,
        std::uint8_t green,
        std::uint8_t blue,
        std::uint8_t alpha = 255)
        : r(red),
          g(green),
          b(blue),
          a(alpha)
    {
    }

    [[nodiscard]] constexpr bool operator==(const Color& other) const
    {
        return r == other.r
            && g == other.g
            && b == other.b
            && a == other.a;
    }

    [[nodiscard]] constexpr bool operator!=(const Color& other) const
    {
        return !(*this == other);
    }

    [[nodiscard]] constexpr bool isOpaque() const
    {
        return a == 255;
    }

    [[nodiscard]] constexpr bool isTransparent() const
    {
        return a == 0;
    }
};

} // namespace creature_studio::core