#include <creature_studio/painting/creature_part_converter.hpp>

#include <cstddef>

namespace creature_studio::painting
{

domain::CreaturePart createCreaturePart(
    const PaintLayer& layer)
{
    domain::CreaturePart part;

    part.name = layer.name();
    part.width = layer.width();
    part.height = layer.height();

    part.imageData.reserve(
        layer.width() * layer.height() * 4);

    for (std::size_t y = 0; y < layer.height(); ++y)
    {
        for (std::size_t x = 0; x < layer.width(); ++x)
        {
            const Pixel& pixel = layer.pixel(x, y);

            part.imageData.push_back(pixel.red);
            part.imageData.push_back(pixel.green);
            part.imageData.push_back(pixel.blue);
            part.imageData.push_back(pixel.alpha);
        }
    }

    return part;
}

} // namespace creature_studio::painting
