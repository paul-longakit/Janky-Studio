#include <catch2/catch_test_macros.hpp>

#include <creature_studio/domain/state.hpp>
#include <creature_studio/domain/state_machine.hpp>
#include <creature_studio/domain/state_transition.hpp>
#include <creature_studio/domain/state_type.hpp>

#include <creature_studio/statemachine/state_machine_runner.hpp>

namespace
{

creature_studio::domain::StateMachine createStateMachine()
{
    using namespace creature_studio::domain;

    State idle;
    idle.id = 1;
    idle.name = "Idle";
    idle.type = StateType::Idle;

    State attack;
    attack.id = 2;
    attack.name = "Attack";
    attack.type = StateType::Attack;

    idle.transitions.push_back(
        StateTransition{
            .targetStateId = attack.id,
            .conditionTag = "enemy_in_range"
        });

    StateMachine stateMachine;
    stateMachine.states = {idle, attack};
    stateMachine.initialStateId = idle.id;

    return stateMachine;
}

} // namespace

TEST_CASE("State machine starts in its initial state")
{
    const auto stateMachine = createStateMachine();

    creature_studio::statemachine::StateMachineRunner runner{
        stateMachine};

    REQUIRE(runner.currentState() != nullptr);
    REQUIRE(runner.currentState()->name == "Idle");
    REQUIRE(runner.context().currentStateId == 1);
    REQUIRE(runner.context().timeInState == 0.0);
}

TEST_CASE("State machine transitions when condition matches")
{
    const auto stateMachine = createStateMachine();

    creature_studio::statemachine::StateMachineRunner runner{
        stateMachine};

    const bool transitioned =
        runner.trigger("enemy_in_range");

    REQUIRE(transitioned);
    REQUIRE(runner.currentState() != nullptr);
    REQUIRE(runner.currentState()->name == "Attack");
    REQUIRE(runner.context().currentStateId == 2);
}

TEST_CASE("State machine ignores unknown condition")
{
    const auto stateMachine = createStateMachine();

    creature_studio::statemachine::StateMachineRunner runner{
        stateMachine};

    const bool transitioned =
        runner.trigger("unknown_condition");

    REQUIRE_FALSE(transitioned);
    REQUIRE(runner.currentState() != nullptr);
    REQUIRE(runner.currentState()->name == "Idle");
}

TEST_CASE("State machine advances time in current state")
{
    const auto stateMachine = createStateMachine();

    creature_studio::statemachine::StateMachineRunner runner{
        stateMachine};

    runner.update(0.5);

    REQUIRE(runner.context().timeInState == 0.5);
}

TEST_CASE("State transition resets state time")
{
    const auto stateMachine = createStateMachine();

    creature_studio::statemachine::StateMachineRunner runner{
        stateMachine};

    runner.update(2.0);

    REQUIRE(runner.context().timeInState == 2.0);

    const bool transitioned =
        runner.trigger("enemy_in_range");

    REQUIRE(transitioned);
    REQUIRE(runner.context().timeInState == 0.0);
}