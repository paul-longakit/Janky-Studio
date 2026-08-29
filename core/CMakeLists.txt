add_library(janky_studio_core STATIC
    src/vec2.cpp
    src/rect.cpp
    src/color.cpp
    src/unique_id.cpp
    src/result.cpp
)

target_include_directories(janky_studio_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(janky_studio_core
    PUBLIC
        cxx_std_23
)