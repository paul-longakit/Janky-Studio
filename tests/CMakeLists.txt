add_executable(janky_studio_tests
    statemachine/state_machine_runner_tests.cpp
    
    domain/creature_validator_tests.cpp

    painting/paint_document_tests.cpp
    painting/brush_tests.cpp
    painting/animation_frame_converter_tests.cpp
)

target_link_libraries(janky_studio_tests
    PRIVATE
        janky_studio_statemachine
        janky_studio_domain
        janky_studio_painting
        Catch2::Catch2WithMain
)

target_compile_features(janky_studio_tests
    PRIVATE
        cxx_std_23
)

include(Catch)

catch_discover_tests(janky_studio_tests)