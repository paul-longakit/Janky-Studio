add_library(janky_studio_ui STATIC)

set_target_properties(janky_studio_ui PROPERTIES
    AUTOMOC ON
)

target_sources(janky_studio_ui
    PRIVATE
        src/main_menu_page.cpp
        src/creature_editor_page.cpp
        src/state_editor_page.cpp
        src/arena_editor_page.cpp
        src/simulation_page.cpp
        src/paint_canvas.cpp

        include/creature_studio/ui/pages/main_menu_page.hpp
        include/creature_studio/ui/pages/creature_editor_page.hpp
        include/creature_studio/ui/pages/state_editor_page.hpp
        include/creature_studio/ui/pages/arena_editor_page.hpp
        include/creature_studio/ui/pages/simulation_page.hpp
        include/creature_studio/ui/widgets/paint_canvas.hpp
)

target_include_directories(janky_studio_ui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(janky_studio_ui
    PUBLIC
        Qt6::Widgets
        janky_studio_domain
        janky_studio_painting
)