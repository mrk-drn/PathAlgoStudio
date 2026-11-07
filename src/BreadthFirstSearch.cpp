
#include "Pathfinding.hpp"
#include "Grid.hpp"
#include "Cell.hpp"

#include <vector>
#include <queue>
#include <algorithm>

// ============================================================
// BREADTH-FIRST SEARCH (GRAPH SEARCH)
//
// Frontier: FIFO Queue
// Movement: 4 neighbours in 2D, 6 neighbours in 3D
// Guarantees: Shortest path for unweighted grids
// ============================================================

SearchResult breadthFirstSearch(
    const Grid& grid,
    int start,
    int end
)
{
    SearchResult result;

    const int totalCells = grid.getSize();

    if (
        !grid.isValidIndex(start) ||
        !grid.isValidIndex(end)
    )
    {
        return result;
    }

    // Start and end must not be barriers.
    if (
        grid.isBarrier(start) ||
        grid.isBarrier(end)
    )
    {
        return result;
    }

    // Special case: start and end are identical.
    if (start == end)
    {
        result.found = true;
        result.solutionPath.push_back(start);

        return result;
    }

    // FIFO Frontier

    std::queue<int> frontier;

    frontier.push(start);

    // Initialize Data Structures

    std::vector<bool> explored(totalCells, false);

    std::vector<bool> inFrontier(totalCells, false);

    std::vector<int> parent(totalCells, -1);

    inFrontier[start] = true;


    // Main search loop

    while (!frontier.empty())
    {
        // Retrieve first element (FIFO).
        const int current = frontier.front();

        frontier.pop();

        inFrontier[current] = false;

        // Skip already explored cells.
        if (explored[current])
        {
            continue;
        }

        // Mark current cell as explored.
        explored[current] = true;

        // Record exploration order for visualization.
        result.exploredOrder.push_back(current);

        // Check goal

        if (current == end)
        {
            result.found = true;

            // Reconstruct solution path.
            int currentPathCell = end;

            while (currentPathCell != -1)
            {
                result.solutionPath.push_back(
                    currentPathCell
                );

                currentPathCell = parent[currentPathCell];
            }

            // Reverse to obtain start -> end.
            std::reverse(
                result.solutionPath.begin(),
                result.solutionPath.end()
            );

            return result;
        }

        // Get neighbours

        const std::vector<int> neighbors =
            grid.getNeighbors(current);

        // Explore neighbours

        for (int neighbor : neighbors)
        {
            // Validate neighbour index.
            if (!grid.isValidIndex(neighbor))
            {
                continue;
            }

            // Skip barriers.
            if (grid.isBarrier(neighbor))
            {
                continue;
            }

            // Skip explored or already queued cells.
            if (
                explored[neighbor] ||
                inFrontier[neighbor]
            )
            {
                continue;
            }

            // Record parent.
            parent[neighbor] = current;

            // Add neighbour to FIFO queue.
            frontier.push(neighbor);

            inFrontier[neighbor] = true;
        }
    }

    // No path found
    result.found = false;

    return result;
}
