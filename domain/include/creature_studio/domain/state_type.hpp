#pragma once

namespace creature_studio::domain
{

enum class StateType
{
    Idle,
    Movement,
    Attack,
    Defense,
    Dodge,
    Hit,
    Death,
    Special
};

} // namespace creature_studio::domain
