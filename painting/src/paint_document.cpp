#include <creature_studio/painting/paint_document.hpp>

#include <stdexcept>
#include <utility>

namespace creature_studio::painting
{

PaintDocument::PaintDocument(
    std::size_t width,
    std::size_t height)
    : m_width(width)
    , m_height(height)
{
}

std::size_t PaintDocument::width() const
{
    return m_width;
}

std::size_t PaintDocument::height() const
{
    return m_height;
}

std::size_t PaintDocument::frameCount() const
{
    return m_frames.size();
}

PaintFrame& PaintDocument::frame(std::size_t index)
{
    if (index >= m_frames.size())
    {
        throw std::out_of_range(
            "PaintDocument frame index out of range.");
    }

    return m_frames[index];
}

const PaintFrame& PaintDocument::frame(std::size_t index) const
{
    if (index >= m_frames.size())
    {
        throw std::out_of_range(
            "PaintDocument frame index out of range.");
    }

    return m_frames[index];
}

PaintFrame& PaintDocument::addFrame()
{
    m_frames.emplace_back(
        m_width,
        m_height);

    m_activeFrameIndex = m_frames.size() - 1;

    return m_frames.back();
}

void PaintDocument::removeFrame(std::size_t index)
{
    if (index >= m_frames.size())
    {
        throw std::out_of_range(
            "PaintDocument frame index out of range.");
    }

    m_frames.erase(
        m_frames.begin() +
        static_cast<std::ptrdiff_t>(index));

    if (m_frames.empty())
    {
        m_activeFrameIndex = 0;
        return;
    }

    if (m_activeFrameIndex >= m_frames.size())
    {
        m_activeFrameIndex = m_frames.size() - 1;
    }
}

std::size_t PaintDocument::activeFrameIndex() const
{
    return m_activeFrameIndex;
}

void PaintDocument::setActiveFrameIndex(std::size_t index)
{
    if (index >= m_frames.size())
    {
        throw std::out_of_range(
            "PaintDocument active frame index out of range.");
    }

    m_activeFrameIndex = index;
}

std::size_t PaintDocument::layerCount() const
{
    if (m_frames.empty())
    {
        return 0;
    }

    return m_frames[m_activeFrameIndex].layerCount();
}

PaintLayer& PaintDocument::layer(std::size_t index)
{
    if (m_frames.empty())
    {
        throw std::out_of_range(
            "PaintDocument has no frames.");
    }

    return m_frames[m_activeFrameIndex].layer(index);
}

const PaintLayer& PaintDocument::layer(std::size_t index) const
{
    if (m_frames.empty())
    {
        throw std::out_of_range(
            "PaintDocument has no frames.");
    }

    return m_frames[m_activeFrameIndex].layer(index);
}

PaintLayer& PaintDocument::addLayer(std::string name)
{
    if (m_frames.empty())
    {
        addFrame();
    }

    return m_frames[m_activeFrameIndex].addLayer(
        std::move(name));
}

void PaintDocument::removeLayer(std::size_t index)
{
    if (m_frames.empty())
    {
        throw std::out_of_range(
            "PaintDocument has no frames.");
    }

    m_frames[m_activeFrameIndex].removeLayer(index);
}

} // namespace creature_studio::painting