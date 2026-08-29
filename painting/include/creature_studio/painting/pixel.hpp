#pragma once

#include <cstdint>

namespace creature_studio::painting
{

struct Pixel
{
    std::uint8_t red{0};
    std::uint8_t green{0};
    std::uint8_t blue{0};
    std::uint8_t alpha{0};
};

} // namespace creature_studio::painting