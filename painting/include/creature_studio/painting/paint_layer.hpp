#pragma once

#include <creature_studio/painting/pixel.hpp>

#include <cstddef>
#include <string>
#include <vector>

namespace creature_studio::painting
{

class PaintLayer
{
public:
    PaintLayer(
        std::string name,
        std::size_t width,
        std::size_t height);

    const std::string& name() const;

    void setName(std::string name);

    std::size_t width() const;
    std::size_t height() const;

    Pixel& pixel(
        std::size_t x,
        std::size_t y);

    const Pixel& pixel(
        std::size_t x,
        std::size_t y) const;

private:
    std::string m_name;
    std::size_t m_width;
    std::size_t m_height;
    std::vector<Pixel> m_pixels;
};

} // namespace creature_studio::painting