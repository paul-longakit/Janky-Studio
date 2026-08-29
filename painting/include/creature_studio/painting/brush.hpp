#pragma once

#include <creature_studio/painting/brush_settings.hpp>
#include <creature_studio/painting/paint_layer.hpp>

#include <cstddef>

namespace creature_studio::painting
{

class Brush
{
public:
    explicit Brush(BrushSettings settings = {});

    const BrushSettings& settings() const;

    void setSettings(BrushSettings settings);

    void paint(
        PaintLayer& layer,
        std::size_t centerX,
        std::size_t centerY);

private:
    BrushSettings m_settings;
};

} // namespace creature_studio::painting