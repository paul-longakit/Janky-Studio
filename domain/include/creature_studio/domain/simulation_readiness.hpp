#pragma once

#include <creature_studio/domain/validation_error.hpp>

#include <vector>

namespace creature_studio::domain
{

enum class SimulationReadiness
{
    Ready,
    Warning,
    Error
};

} // namespace creature_studio::domain
