#include <creature_studio/painting/paint_frame.hpp>

#include <stdexcept>
#include <utility>

namespace creature_studio::painting
{

PaintFrame::PaintFrame(
    std::size_t width,
    std::size_t height)
    : m_width(width)
    , m_height(height)
{
}

std::size_t PaintFrame::width() const
{
    return m_width;
}

std::size_t PaintFrame::height() const
{
    return m_height;
}

std::size_t PaintFrame::layerCount() const
{
    return m_layers.size();
}

PaintLayer& PaintFrame::layer(std::size_t index)
{
    if (index >= m_layers.size())
    {
        throw std::out_of_range(
            "PaintFrame layer index out of range.");
    }

    return m_layers[index];
}

const PaintLayer& PaintFrame::layer(std::size_t index) const
{
    if (index >= m_layers.size())
    {
        throw std::out_of_range(
            "PaintFrame layer index out of range.");
    }

    return m_layers[index];
}

PaintLayer& PaintFrame::addLayer(std::string name)
{
    m_layers.emplace_back(
        std::move(name),
        m_width,
        m_height);

    return m_layers.back();
}

void PaintFrame::removeLayer(std::size_t index)
{
    if (index >= m_layers.size())
    {
        throw std::out_of_range(
            "PaintFrame layer index out of range.");
    }

    m_layers.erase(
        m_layers.begin() +
        static_cast<std::ptrdiff_t>(index));
}

} // namespace creature_studio::painting