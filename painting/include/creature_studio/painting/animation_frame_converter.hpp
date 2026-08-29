#pragma once

#include <creature_studio/domain/animation_frame.hpp>
#include <creature_studio/painting/paint_layer.hpp>

#include <cstdint>

namespace creature_studio::painting
{

domain::AnimationFrame createAnimationFrame(
    const PaintLayer& layer,
    std::uint32_t frameIndex,
    double duration);

} // namespace creature_studio::painting