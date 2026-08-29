#pragma once

#include <creature_studio/domain/state.hpp>

#include <string_view>

namespace creature_studio::statemachine
{

class TransitionEvaluator
{
public:
    static const domain::StateTransition* evaluate(
        const domain::State& state,
        std::string_view conditionTag);
};

} // namespace creature_studio::statemachine