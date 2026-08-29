#include <creature_studio/painting/paint_layer.hpp>

#include <stdexcept>
#include <utility>

namespace creature_studio::painting
{

PaintLayer::PaintLayer(
    std::string name,
    std::size_t width,
    std::size_t height)
    : m_name(std::move(name))
    , m_width(width)
    , m_height(height)
    , m_pixels(width * height)
{
}

const std::string& PaintLayer::name() const
{
    return m_name;
}

void PaintLayer::setName(std::string name)
{
    m_name = std::move(name);
}

std::size_t PaintLayer::width() const
{
    return m_width;
}

std::size_t PaintLayer::height() const
{
    return m_height;
}

Pixel& PaintLayer::pixel(
    std::size_t x,
    std::size_t y)
{
    if (x >= m_width || y >= m_height)
    {
        throw std::out_of_range("PaintLayer pixel coordinates out of range.");
    }

    return m_pixels[y * m_width + x];
}

const Pixel& PaintLayer::pixel(
    std::size_t x,
    std::size_t y) const
{
    if (x >= m_width || y >= m_height)
    {
        throw std::out_of_range("PaintLayer pixel coordinates out of range.");
    }

    return m_pixels[y * m_width + x];
}

} // namespace creature_studio::painting