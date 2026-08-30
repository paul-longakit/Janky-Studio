#pragma once

#include <creature_studio/core/unique_id.hpp>
#include <creature_studio/domain/creature_pivot.hpp>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace creature_studio::domain
{

struct CreaturePart
{
    core::UniqueId id{};
    std::string name;

    std::size_t width{0};
    std::size_t height{0};
    std::vector<std::uint8_t> imageData;

    CreaturePivot pivot;

    std::optional<core::UniqueId> parentPartId;
};

} // namespace creature_studio::domain
