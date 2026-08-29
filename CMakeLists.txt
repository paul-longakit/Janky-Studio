cmake_minimum_required(VERSION 3.28)

project(JankyStudio
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Catch2 CONFIG REQUIRED)

include(CTest)
enable_testing()

add_subdirectory(core)
add_subdirectory(domain)
add_subdirectory(statemachine)
add_subdirectory(painting)
add_subdirectory(ui)
add_subdirectory(app/creature_studio)
add_subdirectory(tests)