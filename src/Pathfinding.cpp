
#include "Pathfinding.hpp"


SearchResult runSearch(
    AlgorithmType algorithm,
    const Grid& grid,
    int start,
    int end
) {
    switch (algorithm) {

        case AlgorithmType::DFS:
            return depthFirstSearch(grid, start, end);

        case AlgorithmType::BFS:
            return breadthFirstSearch(grid, start, end);

        case AlgorithmType::ASTAR:
            return aStarSearch(grid, start, end);

        case AlgorithmType::GREEDY_BEST_FIRST:
            return greedyBestFirstSearch(grid, start, end);

        case AlgorithmType::DIJKSTRA:
            return uniformCostSearch(grid, start, end);
    }

    return {};
}

std::string getAlgorithmName(AlgorithmType algorithm) {

    switch (algorithm) {

        case AlgorithmType::DFS:
            return "Depth-First Search";

        case AlgorithmType::BFS:
            return "Breadth-First Search";

        case AlgorithmType::ASTAR:
            return "A* Search";

        case AlgorithmType::GREEDY_BEST_FIRST:
            return "Greedy Best-First Search";

        case AlgorithmType::DIJKSTRA:
            return "Uniform-Cost Search (Dijkstra)";
    }

    return "Unknown";
}
