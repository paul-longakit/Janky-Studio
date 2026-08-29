#pragma once

#include <creature_studio/domain/creature.hpp>
#include <creature_studio/domain/simulation_readiness.hpp>
#include <creature_studio/domain/validation_error.hpp>

#include <vector>

namespace creature_studio::domain
{

struct ValidationResult
{
    SimulationReadiness readiness{SimulationReadiness::Ready};
    std::vector<ValidationError> errors;
};

ValidationResult validateForSimulation(const Creature& creature);

} // namespace creature_studio::domain