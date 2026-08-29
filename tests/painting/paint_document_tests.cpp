#include <catch2/catch_test_macros.hpp>

#include <creature_studio/painting/paint_document.hpp>

using namespace creature_studio::painting;

TEST_CASE("Paint document stores its dimensions")
{
    PaintDocument document{32, 24};

    REQUIRE(document.width() == 32);
    REQUIRE(document.height() == 24);
    REQUIRE(document.layerCount() == 0);
}

TEST_CASE("Paint document can add a layer")
{
    PaintDocument document{16, 16};

    auto& layer = document.addLayer("Body");

    REQUIRE(document.layerCount() == 1);
    REQUIRE(layer.name() == "Body");
    REQUIRE(layer.width() == 16);
    REQUIRE(layer.height() == 16);
}

TEST_CASE("Paint layer stores pixels")
{
    PaintDocument document{8, 8};

    auto& layer = document.addLayer("Body");

    auto& pixel = layer.pixel(2, 3);

    pixel.red = 255;
    pixel.green = 128;
    pixel.blue = 64;
    pixel.alpha = 255;

    REQUIRE(layer.pixel(2, 3).red == 255);
    REQUIRE(layer.pixel(2, 3).green == 128);
    REQUIRE(layer.pixel(2, 3).blue == 64);
    REQUIRE(layer.pixel(2, 3).alpha == 255);
}

TEST_CASE("Paint layer rejects invalid pixel coordinates")
{
    PaintDocument document{8, 8};

    auto& layer = document.addLayer("Body");

    REQUIRE_THROWS(layer.pixel(8, 0));
    REQUIRE_THROWS(layer.pixel(0, 8));
}

TEST_CASE("Paint document can remove a layer")
{
    PaintDocument document{16, 16};

    document.addLayer("Body");
    document.addLayer("Outline");

    REQUIRE(document.layerCount() == 2);

    document.removeLayer(0);

    REQUIRE(document.layerCount() == 1);
    REQUIRE(document.layer(0).name() == "Outline");
}

TEST_CASE("Paint document can add a frame")
{
    PaintDocument document{16, 16};

    REQUIRE(document.frameCount() == 0);

    auto& frame = document.addFrame();

    REQUIRE(document.frameCount() == 1);
    REQUIRE(frame.width() == 16);
    REQUIRE(frame.height() == 16);
}

TEST_CASE("Paint document layers belong to the active frame")
{
    PaintDocument document{16, 16};

    document.addFrame();
    document.addLayer("Body");

    REQUIRE(document.frameCount() == 1);
    REQUIRE(document.layerCount() == 1);
    REQUIRE(document.layer(0).name() == "Body");
}

TEST_CASE("Paint document can switch active frame")
{
    PaintDocument document{16, 16};

    document.addFrame();
    document.addLayer("Body");

    document.addFrame();
    document.addLayer("Outline");

    REQUIRE(document.activeFrameIndex() == 1);
    REQUIRE(document.layer(0).name() == "Outline");

    document.setActiveFrameIndex(0);

    REQUIRE(document.activeFrameIndex() == 0);
    REQUIRE(document.layer(0).name() == "Body");
}