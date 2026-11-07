
#pragma once

#include "Grid.hpp"

#include <vector>
#include <string>

struct SearchResult {
    bool found = false;

    // Order in which cells were explored.
    std::vector<int> exploredOrder;

    // Cells belonging to the final solution path.
    std::vector<int> solutionPath;
};

enum class AlgorithmType {
    DFS,
    BFS,
    ASTAR,
    GREEDY_BEST_FIRST,
    DIJKSTRA
};

SearchResult depthFirstSearch(
    const Grid& grid, int start, int end
);

SearchResult breadthFirstSearch(
    const Grid& grid, int start, int end
);

SearchResult aStarSearch(
    const Grid& grid, int start, int end
);

SearchResult greedyBestFirstSearch(
    const Grid& grid, int start, int end
);

SearchResult uniformCostSearch(
    const Grid& grid, int start, int end
);

SearchResult runSearch(
    AlgorithmType algorithm,
    const Grid& grid,
    int start,
    int end
);

std::string getAlgorithmName(AlgorithmType algorithm);
