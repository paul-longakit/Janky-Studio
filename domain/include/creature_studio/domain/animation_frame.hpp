#pragma once

#include <cstdint>
#include <vector>

namespace creature_studio::domain
{

struct AnimationFrame
{
    std::uint32_t frameIndex{0};
    std::vector<std::uint8_t> imageData;
    double duration{0.0};
};

} // namespace creature_studio::domain
