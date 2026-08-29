#include <creature_studio/painting/animation_frame_converter.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Paint layer converts to animation frame")
{
    creature_studio::painting::PaintLayer layer(
        "Body",
        2,
        1);

    auto& first = layer.pixel(0, 0);
    first.red = 255;
    first.green = 128;
    first.blue = 64;
    first.alpha = 255;

    auto& second = layer.pixel(1, 0);
    second.red = 10;
    second.green = 20;
    second.blue = 30;
    second.alpha = 40;

    const auto frame =
        creature_studio::painting::createAnimationFrame(
            layer,
            3,
            0.125);

    REQUIRE(frame.frameIndex == 3);
    REQUIRE(frame.duration == 0.125);

    REQUIRE(frame.imageData.size() == 8);

    REQUIRE(frame.imageData[0] == 255);
    REQUIRE(frame.imageData[1] == 128);
    REQUIRE(frame.imageData[2] == 64);
    REQUIRE(frame.imageData[3] == 255);

    REQUIRE(frame.imageData[4] == 10);
    REQUIRE(frame.imageData[5] == 20);
    REQUIRE(frame.imageData[6] == 30);
    REQUIRE(frame.imageData[7] == 40);
}