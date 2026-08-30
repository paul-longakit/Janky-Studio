#pragma once

#include <creature_studio/domain/creature_part.hpp>
#include <creature_studio/painting/paint_layer.hpp>

namespace creature_studio::painting
{

domain::CreaturePart createCreaturePart(
    const PaintLayer& layer);

} // namespace creature_studio::painting
