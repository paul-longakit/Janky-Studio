add_library(janky_studio_statemachine STATIC
    include/creature_studio/statemachine/state_context.hpp
    include/creature_studio/statemachine/transition_evaluator.hpp
    include/creature_studio/statemachine/state_machine_runner.hpp
    src/transition_evaluator.cpp
    src/state_machine_runner.cpp
)

target_include_directories(janky_studio_statemachine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(janky_studio_statemachine
    PUBLIC
        janky_studio_domain
)

target_compile_features(janky_studio_statemachine
    PUBLIC
        cxx_std_23
)