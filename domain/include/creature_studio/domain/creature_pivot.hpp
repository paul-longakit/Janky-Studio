#pragma once

namespace creature_studio::domain
{

// Position of a part's transform origin in the part's local coordinates.
struct CreaturePivot
{
    double x{0.0};
    double y{0.0};
};

} // namespace creature_studio::domain
