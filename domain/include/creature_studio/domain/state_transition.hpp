#pragma once

#include <creature_studio/core/unique_id.hpp>

#include <string>

namespace creature_studio::domain
{

struct StateTransition
{
    core::UniqueId targetStateId{0};
    std::string conditionTag;
};

} // namespace creature_studio::domain
