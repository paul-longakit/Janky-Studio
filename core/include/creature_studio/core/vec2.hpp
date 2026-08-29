#pragma once

namespace creature_studio::core
{

struct Vec2
{
    float x{0.0F};
    float y{0.0F};

    constexpr Vec2() = default;

    constexpr Vec2(float xValue, float yValue)
        : x(xValue),
          y(yValue)
    {
    }

    [[nodiscard]] constexpr Vec2 operator+(const Vec2& other) const
    {
        return {x + other.x, y + other.y};
    }

    [[nodiscard]] constexpr Vec2 operator-(const Vec2& other) const
    {
        return {x - other.x, y - other.y};
    }

    [[nodiscard]] constexpr Vec2 operator*(float scalar) const
    {
        return {x * scalar, y * scalar};
    }

    [[nodiscard]] constexpr Vec2 operator/(float scalar) const
    {
        return {x / scalar, y / scalar};
    }

    constexpr Vec2& operator+=(const Vec2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr Vec2& operator-=(const Vec2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    constexpr Vec2& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    constexpr Vec2& operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    [[nodiscard]] constexpr bool operator==(const Vec2& other) const
    {
        return x == other.x && y == other.y;
    }

    [[nodiscard]] constexpr bool operator!=(const Vec2& other) const
    {
        return !(*this == other);
    }
};

[[nodiscard]] constexpr Vec2 operator*(float scalar, const Vec2& vector)
{
    return vector * scalar;
}

} // namespace creature_studio::core