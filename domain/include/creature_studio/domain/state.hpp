#pragma once

#include <creature_studio/core/unique_id.hpp>
#include <creature_studio/domain/state_transition.hpp>
#include <creature_studio/domain/state_type.hpp>

#include <string>
#include <vector>

namespace creature_studio::domain
{

struct State
{
    core::UniqueId id{0};
    std::string name;
    StateType type{StateType::Idle};
    std::string animationName;
    std::vector<StateTransition> transitions;
};

} // namespace creature_studio::domain
