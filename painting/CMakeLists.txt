add_library(janky_studio_painting STATIC)

target_sources(janky_studio_painting
    PRIVATE
        src/paint_layer.cpp
        src/paint_frame.cpp
        src/paint_document.cpp
        src/brush.cpp
        src/animation_frame_converter.cpp
        src/creature_part_converter.cpp

        include/creature_studio/painting/pixel.hpp
        include/creature_studio/painting/paint_layer.hpp
        include/creature_studio/painting/paint_frame.hpp
        include/creature_studio/painting/paint_document.hpp
        include/creature_studio/painting/brush_settings.hpp
        include/creature_studio/painting/brush.hpp
        include/creature_studio/painting/animation_frame_converter.hpp
        include/creature_studio/painting/creature_part_converter.hpp
)

target_include_directories(janky_studio_painting
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(janky_studio_painting
    PUBLIC
        janky_studio_core
        janky_studio_domain
)

target_compile_features(janky_studio_painting
    PUBLIC
        cxx_std_23
)