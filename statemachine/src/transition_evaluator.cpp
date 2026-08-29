#include <creature_studio/statemachine/transition_evaluator.hpp>

namespace creature_studio::statemachine
{

const domain::StateTransition* TransitionEvaluator::evaluate(
    const domain::State& state,
    std::string_view conditionTag)
{
    for (const auto& transition : state.transitions)
    {
        if (transition.conditionTag == conditionTag)
        {
            return &transition;
        }
    }

    return nullptr;
}

} // namespace creature_studio::statemachine