#include <creature_studio/domain/creature_validator.hpp>

#include <algorithm>
#include <unordered_set>

namespace creature_studio::domain
{

ValidationResult validateForSimulation(const Creature& creature)
{
    ValidationResult result;

    const auto& stateMachine = creature.stateMachine;

    if (stateMachine.states.empty())
    {
        result.readiness = SimulationReadiness::Error;
        result.errors.push_back(
            {"Creature must contain at least one state."});
        return result;
    }

    std::unordered_set<core::UniqueId> stateIds;

    for (const auto& state : stateMachine.states)
    {
        stateIds.insert(state.id);
    }

    if (stateIds.find(stateMachine.initialStateId) == stateIds.end())
    {
        result.readiness = SimulationReadiness::Error;
        result.errors.push_back(
            {"Initial state does not reference a valid state."});
    }

    const auto idleIt = std::find_if(
        stateMachine.states.begin(),
        stateMachine.states.end(),
        [](const State& state)
        {
            return state.type == StateType::Idle;
        });

    if (idleIt == stateMachine.states.end())
    {
        result.readiness = SimulationReadiness::Error;
        result.errors.push_back(
            {"Creature must contain an Idle state."});
    }

    for (const auto& state : stateMachine.states)
    {
        if (!state.animationName.empty())
        {
            const auto animationIt = std::find_if(
                creature.animations.begin(),
                creature.animations.end(),
                [&state](const Animation& animation)
                {
                    return animation.name == state.animationName;
                });

            if (animationIt == creature.animations.end())
            {
                result.readiness = SimulationReadiness::Error;
                result.errors.push_back(
                    {"State '" + state.name +
                     "' references a missing animation '" +
                     state.animationName + "'."});
            }
        }

        for (const auto& transition : state.transitions)
        {
            if (stateIds.find(transition.targetStateId) == stateIds.end())
            {
                result.readiness = SimulationReadiness::Error;
                result.errors.push_back(
                    {"State '" + state.name +
                     "' contains a transition to a missing state."});
            }
        }
    }

    for (const auto& animation : creature.animations)
    {
        if (animation.frames.empty())
        {
            result.readiness = SimulationReadiness::Error;
            result.errors.push_back(
                {"Animation '" + animation.name +
                "' contains no frames."});

            continue;
        }

        for (const auto& frame : animation.frames)
        {
            if (frame.imageData.empty())
            {
                result.readiness = SimulationReadiness::Error;
                result.errors.push_back(
                    {"Animation '" + animation.name +
                    "' contains a frame with no image data."});
            }

            if (frame.duration <= 0.0)
            {
                result.readiness = SimulationReadiness::Error;
                result.errors.push_back(
                    {"Animation '" + animation.name +
                    "' contains a frame with an invalid duration."});
            }
        }
    }

    return result;
}

} // namespace creature_studio::domain