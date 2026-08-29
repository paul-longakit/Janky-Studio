#include <creature_studio/statemachine/state_machine_runner.hpp>

#include <creature_studio/statemachine/transition_evaluator.hpp>

namespace creature_studio::statemachine
{

StateMachineRunner::StateMachineRunner(
    const domain::StateMachine& stateMachine)
    : m_stateMachine(stateMachine)
{
    reset();
}

void StateMachineRunner::reset()
{
    m_context.currentStateId = m_stateMachine.initialStateId;
    m_context.timeInState = 0.0;
}

void StateMachineRunner::update(double deltaTime)
{
    if (deltaTime <= 0.0)
    {
        return;
    }

    m_context.timeInState += deltaTime;
}

bool StateMachineRunner::trigger(std::string_view conditionTag)
{
    const auto* state = currentState();

    if (state == nullptr)
    {
        return false;
    }

    const auto* transition =
        TransitionEvaluator::evaluate(*state, conditionTag);

    if (transition == nullptr)
    {
        return false;
    }

    m_context.currentStateId = transition->targetStateId;
    m_context.timeInState = 0.0;

    return true;
}

const domain::State* StateMachineRunner::currentState() const
{
    for (const auto& state : m_stateMachine.states)
    {
        if (state.id == m_context.currentStateId)
        {
            return &state;
        }
    }

    return nullptr;
}

const StateContext& StateMachineRunner::context() const
{
    return m_context;
}

} // namespace creature_studio::statemachine