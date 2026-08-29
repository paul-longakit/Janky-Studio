#pragma once

#include <creature_studio/core/unique_id.hpp>

namespace creature_studio::statemachine
{

struct StateContext
{
    core::UniqueId currentStateId{0};
    double timeInState{0.0};
};

} // namespace creature_studio::statemachine