add_library(janky_studio_domain STATIC
    include/creature_studio/domain/animation.hpp
    include/creature_studio/domain/creature.hpp
    include/creature_studio/domain/animation_frame.hpp
    include/creature_studio/domain/state.hpp
    include/creature_studio/domain/state_machine.hpp
    include/creature_studio/domain/state_transition.hpp
    include/creature_studio/domain/state_type.hpp
    include/creature_studio/domain/simulation_readiness.hpp
    include/creature_studio/domain/validation_error.hpp
    include/creature_studio/domain/creature_validator.hpp
    include/creature_studio/domain/creature_part.hpp
    include/creature_studio/domain/creature_pivot.hpp
    src/creature_validator.cpp
    src/animation_frame.cpp
    src/domain_models.cpp
)

target_include_directories(janky_studio_domain
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(janky_studio_domain
    PUBLIC
        janky_studio_core
)

target_compile_features(janky_studio_domain
    PUBLIC
        cxx_std_23
)
