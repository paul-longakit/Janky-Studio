#include <creature_studio/painting/creature_part_converter.hpp>

TEST_CASE("Paint layer converts to creature part")
{
    using namespace creature_studio::painting;

    PaintLayer layer("Head", 2, 1);

    layer.pixel(0, 0) = Pixel{255, 0, 0, 255};
    layer.pixel(1, 0) = Pixel{0, 255, 0, 128};

    const auto part = createCreaturePart(layer);

    REQUIRE(part.name == "Head");
    REQUIRE(part.width == 2);
    REQUIRE(part.height == 1);
    REQUIRE(part.imageData.size() == 8);

    REQUIRE(part.imageData[0] == 255);
    REQUIRE(part.imageData[1] == 0);
    REQUIRE(part.imageData[2] == 0);
    REQUIRE(part.imageData[3] == 255);

    REQUIRE(part.imageData[4] == 0);
    REQUIRE(part.imageData[5] == 255);
    REQUIRE(part.imageData[6] == 0);
    REQUIRE(part.imageData[7] == 128);
}
