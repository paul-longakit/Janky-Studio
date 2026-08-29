#pragma once

#include "pixel.hpp"

#include <cstddef>

namespace creature_studio::painting
{

struct BrushSettings
{
    std::size_t size{1};
    Pixel color{255, 255, 255, 255};
    double opacity{1.0};
};

} // namespace creature_studio::painting