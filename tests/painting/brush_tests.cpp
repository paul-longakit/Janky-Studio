#include <catch2/catch_test_macros.hpp> 

#include <creature_studio/painting/brush.hpp> 
#include <creature_studio/painting/paint_document.hpp> 
#include <creature_studio/painting/paint_layer.hpp>

using namespace creature_studio::painting;

TEST_CASE("Brush paints a pixel")
{
    PaintDocument document{8, 8};
    auto& layer = document.addLayer("Body");

    BrushSettings settings;
    settings.size = 1;
    settings.color = Pixel{255, 0, 0, 255};

    Brush brush{settings};

    brush.paint(layer, 4, 4);

    REQUIRE(layer.pixel(4, 4).red == 255);
    REQUIRE(layer.pixel(4, 4).green == 0);
    REQUIRE(layer.pixel(4, 4).blue == 0);
    REQUIRE(layer.pixel(4, 4).alpha == 255);
}

TEST_CASE("Brush paints multiple pixels")
{
    PaintDocument document{16, 16};
    auto& layer = document.addLayer("Body");

    BrushSettings settings;
    settings.size = 5;
    settings.color = Pixel{0, 255, 0, 255};

    Brush brush{settings};

    brush.paint(layer, 8, 8);

    std::size_t paintedPixels = 0;

    for (std::size_t y = 0; y < layer.height(); ++y)
    {
        for (std::size_t x = 0; x < layer.width(); ++x)
        {
            if (layer.pixel(x, y).green == 255)
            {
                ++paintedPixels;
            }
        }
    }

    REQUIRE(paintedPixels > 1);
}

TEST_CASE("Brush does not paint outside layer")
{
    PaintDocument document{8, 8};
    auto& layer = document.addLayer("Body");

    BrushSettings settings;
    settings.size = 7;
    settings.color = Pixel{255, 0, 0, 255};

    Brush brush{settings};

    REQUIRE_NOTHROW(
        brush.paint(layer, 0, 0));

    REQUIRE(layer.pixel(0, 0).red == 255);
}

TEST_CASE("Zero size brush does nothing")
{
    PaintDocument document{8, 8};
    auto& layer = document.addLayer("Body");

    BrushSettings settings;
    settings.size = 0;
    settings.color = Pixel{255, 0, 0, 255};

    Brush brush{settings};

    brush.paint(layer, 4, 4);

    REQUIRE(layer.pixel(4, 4).alpha == 0);
}

TEST_CASE("Brush opacity blends with existing pixel")
{
    PaintDocument document{8, 8};
    auto& layer = document.addLayer("Body");

    layer.pixel(4, 4) = Pixel{0, 0, 0, 255};

    BrushSettings settings;
    settings.size = 1;
    settings.color = Pixel{255, 255, 255, 255};
    settings.opacity = 0.5;

    Brush brush{settings};

    brush.paint(layer, 4, 4);

    REQUIRE(layer.pixel(4, 4).red == 128);
    REQUIRE(layer.pixel(4, 4).green == 128);
    REQUIRE(layer.pixel(4, 4).blue == 128);
}

TEST_CASE("Brush settings can be changed")
{
    Brush brush;

    BrushSettings settings;
    settings.size = 8;
    settings.color = Pixel{10, 20, 30, 255};
    settings.opacity = 0.25;

    brush.setSettings(settings);

    REQUIRE(brush.settings().size == 8);
    REQUIRE(brush.settings().color.red == 10);
    REQUIRE(brush.settings().color.green == 20);
    REQUIRE(brush.settings().color.blue == 30);
    REQUIRE(brush.settings().opacity == 0.25);
}