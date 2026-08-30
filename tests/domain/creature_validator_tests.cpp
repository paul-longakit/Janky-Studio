#include <catch2/catch_test_macros.hpp>

#include <creature_studio/domain/creature.hpp>
#include <creature_studio/domain/creature_validator.hpp>
#include <creature_studio/domain/state.hpp>
#include <creature_studio/domain/state_machine.hpp>
#include <creature_studio/domain/state_type.hpp>
#include <creature_studio/domain/state_transition.hpp>
#include <creature_studio/domain/animation.hpp>
#include <creature_studio/domain/animation_frame.hpp>

namespace
{

creature_studio::domain::Creature createValidCreature()
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

    Creature creature;
    creature.name = "Test Creature";
    creature.stateMachine = stateMachine;

    return creature;
}

} // namespace

bool containsErrorMessage(
    const creature_studio::domain::ValidationResult& result,
    const std::string& expected)
{
    for (const auto& error : result.errors)
    {
        if (error.message == expected)
        {
            return true;
        }
    }

    return false;
}

TEST_CASE("Valid creature is ready for simulation")
{
    const auto creature = createValidCreature();

    const auto result = creature_studio::domain::validateForSimulation(
        creature);

    REQUIRE(
        result.readiness ==
        creature_studio::domain::SimulationReadiness::Ready);

    REQUIRE(result.errors.empty());
    REQUIRE(result.readiness ==
            creature_studio::domain::SimulationReadiness::Ready);
}

TEST_CASE("Creature without states is not ready for simulation")
{
    auto creature = createValidCreature();
    creature.stateMachine.states.clear();

    const auto result = creature_studio::domain::validateForSimulation(
        creature);

    REQUIRE(
        result.readiness ==
        creature_studio::domain::SimulationReadiness::Error);

    REQUIRE(result.errors.size() == 1);
    REQUIRE(
        containsErrorMessage(
            result,
            "Creature must contain at least one state."));
}

TEST_CASE("Creature with invalid initial state is not ready")
{
    auto creature = createValidCreature();
    creature.stateMachine.initialStateId = 999;

    const auto result = creature_studio::domain::validateForSimulation(
        creature);

    REQUIRE(
        result.readiness ==
        creature_studio::domain::SimulationReadiness::Error);

    REQUIRE(
    containsErrorMessage(
        result,
        "Initial state does not reference a valid state."));
}

TEST_CASE("Creature without Idle state is not ready")
{
    auto creature = createValidCreature();

    creature.stateMachine.states[0].type =
        creature_studio::domain::StateType::Movement;

    const auto result = creature_studio::domain::validateForSimulation(
        creature);

    REQUIRE(
        result.readiness ==
        creature_studio::domain::SimulationReadiness::Error);

    REQUIRE(
    containsErrorMessage(
        result,
        "Creature must contain an Idle state."));
}

TEST_CASE("State with missing animation is not ready")
{
    auto creature = createValidCreature();

    creature.stateMachine.states[0].animationName = "MissingAnimation";

    const auto result = creature_studio::domain::validateForSimulation(
        creature);

    REQUIRE(
        result.readiness ==
        creature_studio::domain::SimulationReadiness::Error);

    REQUIRE(
    containsErrorMessage(
        result,
        "State 'Idle' references a missing animation 'MissingAnimation'."));
}

TEST_CASE("Transition to missing state is not ready")
{
    auto creature = createValidCreature();

    creature.stateMachine.states[0].transitions[0].targetStateId = 999;

    const auto result = creature_studio::domain::validateForSimulation(
        creature);

    REQUIRE(
        result.readiness ==
        creature_studio::domain::SimulationReadiness::Error);

    REQUIRE(
    containsErrorMessage(
        result,
        "State 'Idle' contains a transition to a missing state."));
}

TEST_CASE("Animation without frames is not ready")
{
    auto creature = createValidCreature();

    creature.animations.push_back(
        creature_studio::domain::Animation{
            .name = "IdleAnimation"
        });

    const auto result =
        creature_studio::domain::validateForSimulation(creature);

    REQUIRE(
        result.readiness ==
        creature_studio::domain::SimulationReadiness::Error);

    REQUIRE(
    containsErrorMessage(
        result,
        "Animation 'IdleAnimation' contains no frames."));
}

TEST_CASE("Animation frame without image data is not ready")
{
    auto creature = createValidCreature();

    creature.animations.push_back(
        creature_studio::domain::Animation{
            .name = "IdleAnimation",
            .frames = {
                creature_studio::domain::AnimationFrame{
                    .frameIndex = 0,
                    .imageData = {},
                    .duration = 0.1
                }
            }
        });

    const auto result =
        creature_studio::domain::validateForSimulation(creature);

    REQUIRE(
        result.readiness ==
        creature_studio::domain::SimulationReadiness::Error);

    REQUIRE(
    containsErrorMessage(
        result,
        "Animation 'IdleAnimation' contains a frame with no image data."));
}

TEST_CASE("Animation frame with invalid duration is not ready")
{
    auto creature = createValidCreature();

    creature.animations.push_back(
        creature_studio::domain::Animation{
            .name = "IdleAnimation",
            .frames = {
                creature_studio::domain::AnimationFrame{
                    .frameIndex = 0,
                    .imageData = {1, 2, 3},
                    .duration = 0.0
                }
            }
        });

    const auto result =
        creature_studio::domain::validateForSimulation(creature);

    REQUIRE(
        result.readiness ==
        creature_studio::domain::SimulationReadiness::Error);

    REQUIRE(
    containsErrorMessage(
        result,
        "Animation 'IdleAnimation' contains a frame with an invalid duration."));
}
TEST_CASE("Creature can contain creature parts")
{
    creature_studio::domain::Creature creature;

    creature_studio::domain::CreaturePart body;
    body.id = 1;
    body.name = "Body";
    body.width = 2;
    body.height = 2;
    body.imageData = {
        255, 0, 0, 255,
        0, 255, 0, 255,
        0, 0, 255, 255,
        255, 255, 255, 255
    };

    creature.parts.push_back(body);

    REQUIRE(creature.parts.size() == 1);
    REQUIRE(creature.parts[0].id == 1);
    REQUIRE(creature.parts[0].name == "Body");
    REQUIRE(creature.parts[0].width == 2);
    REQUIRE(creature.parts[0].height == 2);
    REQUIRE(creature.parts[0].imageData.size() == 16);

    REQUIRE(creature.parts[0].pivot.x == 0.0);
    REQUIRE(creature.parts[0].pivot.y == 0.0);

    creature.parts[0].pivot.x = 1.0;
    creature.parts[0].pivot.y = 1.5;

    REQUIRE(creature.parts[0].pivot.x == 1.0);
    REQUIRE(creature.parts[0].pivot.y == 1.5);
}

TEST_CASE("Creature parts can reference a parent part")
{
    using namespace creature_studio::domain;

    CreaturePart body;
    body.id = 1;
    body.name = "Body";

    CreaturePart head;
    head.id = 2;
    head.name = "Head";
    head.parentPartId = body.id;

    REQUIRE_FALSE(body.parentPartId.has_value());
    REQUIRE(head.parentPartId.has_value());
    REQUIRE(head.parentPartId.value() == body.id);
}
