#pragma once

#include <creature_studio/domain/animation_frame.hpp>

#include <string>
#include <vector>

namespace creature_studio::domain
{

struct Animation
{
    std::string name;
    std::vector<AnimationFrame> frames;
    double fps{0.0};
    bool looping{true};
};

} // namespace creature_studio::domain
