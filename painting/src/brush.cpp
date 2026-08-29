#include <creature_studio/painting/brush.hpp>

#include <algorithm>
#include <cmath>

namespace creature_studio::painting
{

namespace
{

std::uint8_t blendChannel(
    std::uint8_t destination,
    std::uint8_t source,
    double opacity)
{
    const double result =
        static_cast<double>(destination) +
        (
            static_cast<double>(source) -
            static_cast<double>(destination)
        ) * opacity;

    return static_cast<std::uint8_t>(
        std::clamp(
            std::lround(result),
            0L,
            255L));
}

} // namespace

Brush::Brush(BrushSettings settings)
    : m_settings(settings)
{
}

const BrushSettings& Brush::settings() const
{
    return m_settings;
}

void Brush::setSettings(BrushSettings settings)
{
    m_settings = settings;
}

void Brush::paint(
    PaintLayer& layer,
    std::size_t centerX,
    std::size_t centerY)
{
    if (m_settings.size == 0 ||
        layer.width() == 0 ||
        layer.height() == 0)
    {
        return;
    }

    const double radius =
        static_cast<double>(m_settings.size) / 2.0;

    const auto minX = centerX > static_cast<std::size_t>(std::ceil(radius))
        ? centerX - static_cast<std::size_t>(std::ceil(radius))
        : 0;

    const auto minY = centerY > static_cast<std::size_t>(std::ceil(radius))
        ? centerY - static_cast<std::size_t>(std::ceil(radius))
        : 0;

    const auto maxX = std::min(
        layer.width() - 1,
        centerX + static_cast<std::size_t>(std::ceil(radius)));

    const auto maxY = std::min(
        layer.height() - 1,
        centerY + static_cast<std::size_t>(std::ceil(radius)));

    for (std::size_t y = minY; y <= maxY; ++y)
    {
        for (std::size_t x = minX; x <= maxX; ++x)
        {
            const double dx =
                static_cast<double>(x) -
                static_cast<double>(centerX);

            const double dy =
                static_cast<double>(y) -
                static_cast<double>(centerY);

            if ((dx * dx) + (dy * dy) > radius * radius)
            {
                continue;
            }

            auto& pixel = layer.pixel(x, y);

            const double sourceOpacity =
                static_cast<double>(m_settings.color.alpha) / 255.0 *
                std::clamp(m_settings.opacity, 0.0, 1.0);

            pixel.red = blendChannel(
                pixel.red,
                m_settings.color.red,
                sourceOpacity);

            pixel.green = blendChannel(
                pixel.green,
                m_settings.color.green,
                sourceOpacity);

            pixel.blue = blendChannel(
                pixel.blue,
                m_settings.color.blue,
                sourceOpacity);

            pixel.alpha = blendChannel(
                pixel.alpha,
                m_settings.color.alpha,
                sourceOpacity);
        }
    }
}

} // namespace creature_studio::painting