#pragma once

#include <creature_studio/painting/paint_frame.hpp>

#include <cstddef>
#include <string>
#include <vector>

namespace creature_studio::painting
{

class PaintDocument
{
public:
    PaintDocument(
        std::size_t width,
        std::size_t height);

    std::size_t width() const;
    std::size_t height() const;

    std::size_t frameCount() const;

    PaintFrame& frame(std::size_t index);
    const PaintFrame& frame(std::size_t index) const;

    PaintFrame& addFrame();
    void removeFrame(std::size_t index);

    std::size_t activeFrameIndex() const;
    void setActiveFrameIndex(std::size_t index);

    std::size_t layerCount() const;

    PaintLayer& layer(std::size_t index);
    const PaintLayer& layer(std::size_t index) const;

    PaintLayer& addLayer(std::string name);

    void removeLayer(std::size_t index);

private:
    std::size_t m_width;
    std::size_t m_height;

    std::vector<PaintFrame> m_frames;
    std::size_t m_activeFrameIndex{0};
};

} // namespace creature_studio::painting