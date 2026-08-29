#pragma once

#include <creature_studio/domain/state_machine.hpp>
#include <creature_studio/statemachine/state_context.hpp>

#include <string_view>

namespace creature_studio::statemachine
{

class StateMachineRunner
{
public:
    explicit StateMachineRunner(const domain::StateMachine& stateMachine);

    void reset();

    void update(double deltaTime);

    bool trigger(std::string_view conditionTag);

    const domain::State* currentState() const;

    const StateContext& context() const;

private:
    const domain::StateMachine& m_stateMachine;
    StateContext m_context;
};

} // namespace creature_studio::statemachine