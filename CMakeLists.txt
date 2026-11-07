
cmake_minimum_required(VERSION 3.18)

project(PathAlgoStudio LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib CONFIG REQUIRED)

add_executable(PathAlgoStudio

    # Main application
    src/main.cpp

    # Core grid
    src/Grid.cpp

    # Pathfinding algorithms
    src/Pathfinding.cpp
    src/DepthFirstSearch.cpp
    src/BreadthFirstSearch.cpp
    src/AStarSearch.cpp
    src/GreedyBestFirstSearch.cpp
    src/UniformCostSearch.cpp

    # Algorithm management
    src/ComparisonManager.cpp
    src/AlgorithmVisualizer.cpp
)

target_include_directories(PathAlgoStudio PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(PathAlgoStudio PRIVATE
    raylib
)
