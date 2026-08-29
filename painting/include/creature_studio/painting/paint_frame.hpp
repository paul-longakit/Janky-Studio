#pragma once

#include <creature_studio/painting/paint_layer.hpp>

#include <cstddef>
#include <string>
#include <vector>

namespace creature_studio::painting
{

class PaintFrame
{
public:
    PaintFrame(
        std::size_t width,
        std::size_t height);

    std::size_t width() const;
    std::size_t height() const;

    std::size_t layerCount() const;

    PaintLayer& layer(std::size_t index);
    const PaintLayer& layer(std::size_t index) const;

    PaintLayer& addLayer(std::string name);

    void removeLayer(std::size_t index);

private:
    std::size_t m_width;
    std::size_t m_height;
    std::vector<PaintLayer> m_layers;
};

} // namespace creature_studio::painting