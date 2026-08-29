add_executable(janky_studio)

target_sources(janky_studio
    PRIVATE
        src/main.cpp
        src/main_window.cpp
)

target_include_directories(janky_studio
    PRIVATE
        include
)

target_link_libraries(janky_studio
    PRIVATE
        Qt6::Widgets
        janky_studio_ui
)