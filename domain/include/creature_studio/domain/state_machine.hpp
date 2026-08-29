#pragma once

#include <creature_studio/core/unique_id.hpp>
#include <creature_studio/domain/state.hpp>

#include <cstddef>
#include <vector>

namespace creature_studio::domain
{

struct StateMachine
{
    std::vector<State> states;
    core::UniqueId initialStateId{};
    std::size_t maxStates{32};
};

} // namespace creature_studio::domain
