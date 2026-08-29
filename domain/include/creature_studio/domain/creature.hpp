#pragma once

#include <creature_studio/core/unique_id.hpp>
#include <creature_studio/domain/animation.hpp>
#include <creature_studio/domain/state_machine.hpp>

#include <string>
#include <vector>

namespace creature_studio::domain
{

struct Creature
{
    core::UniqueId id{};
    std::string name;
    std::vector<Animation> animations;
    StateMachine stateMachine;
};

} // namespace creature_studio::domain
