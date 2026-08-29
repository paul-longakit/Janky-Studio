#pragma once

#include <creature_studio/core/vec2.hpp>

namespace creature_studio::core
{

struct Rect
{
    float x{0.0F};
    float y{0.0F};
    float width{0.0F};
    float height{0.0F};

    constexpr Rect() = default;

    constexpr Rect(
        float xValue,
        float yValue,
        float widthValue,
        float heightValue)
        : x(xValue),
          y(yValue),
          width(widthValue),
          height(heightValue)
    {
    }

    [[nodiscard]] constexpr Vec2 position() const
    {
        return {x, y};
    }

    [[nodiscard]] constexpr Vec2 size() const
    {
        return {width, height};
    }

    [[nodiscard]] constexpr float left() const
    {
        return x;
    }

    [[nodiscard]] constexpr float right() const
    {
        return x + width;
    }

    [[nodiscard]] constexpr float top() const
    {
        return y;
    }

    [[nodiscard]] constexpr float bottom() const
    {
        return y + height;
    }

    [[nodiscard]] constexpr Vec2 center() const
    {
        return {
            x + width * 0.5F,
            y + height * 0.5F
        };
    }

    [[nodiscard]] constexpr bool contains(const Vec2& point) const
    {
        return point.x >= left()
            && point.x <= right()
            && point.y >= top()
            && point.y <= bottom();
    }

    [[nodiscard]] constexpr bool intersects(const Rect& other) const
    {
        return left() < other.right()
            && right() > other.left()
            && top() < other.bottom()
            && bottom() > other.top();
    }

    [[nodiscard]] constexpr bool operator==(const Rect& other) const
    {
        return x == other.x
            && y == other.y
            && width == other.width
            && height == other.height;
    }

    [[nodiscard]] constexpr bool operator!=(const Rect& other) const
    {
        return !(*this == other);
    }
};

} // namespace creature_studio::core