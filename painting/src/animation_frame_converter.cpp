#include <creature_studio/painting/animation_frame_converter.hpp>

#include <cstddef>

namespace creature_studio::painting
{

domain::AnimationFrame createAnimationFrame(
    const PaintLayer& layer,
    std::uint32_t frameIndex,
    double duration)
{
    domain::AnimationFrame frame;

    frame.frameIndex = frameIndex;
    frame.duration = duration;

    frame.imageData.reserve(
        layer.width() * layer.height() * 4);

    for (std::size_t y = 0; y < layer.height(); ++y)
    {
        for (std::size_t x = 0; x < layer.width(); ++x)
        {
            const Pixel& pixel = layer.pixel(x, y);

            frame.imageData.push_back(pixel.red);
            frame.imageData.push_back(pixel.green);
            frame.imageData.push_back(pixel.blue);
            frame.imageData.push_back(pixel.alpha);
        }
    }

    return frame;
}

} // namespace creature_studio::painting