
#include "Pathfinding.hpp"
#include "Grid.hpp"
#include "Cell.hpp"

#include <vector>
#include <algorithm>

// ============================================================
// DEPTH-FIRST SEARCH (GRAPH SEARCH)
//
// Frontier: LIFO Stack
// Movement: 4 neighbours in 2D, 6 neighbours in 3D
// ============================================================

SearchResult depthFirstSearch(
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

    // LIFO frontier (Stack)

    std::vector<int> frontier;

    frontier.push_back(start);

    // Initialize data structures

    std::vector<bool> explored(totalCells, false);

    std::vector<bool> inFrontier(totalCells, false);

    std::vector<int> parent(totalCells, -1);

    inFrontier[start] = true;

    // MAIN SEARCH LOOP

    while (!frontier.empty())
    {
        // Retrieve last element (LIFO).
        const int current = frontier.back();

        frontier.pop_back();

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

                currentPathCell =
                    parent[currentPathCell];
            }

            // Reverse to obtain start -> end.
            std::reverse(
                result.solutionPath.begin(),
                result.solutionPath.end()
            );

            return result;
        }

        // Explore neighbours

        const std::vector<int> neighbors =
            grid.getNeighbors(current);

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

            // Add neighbour to LIFO stack.
            frontier.push_back(neighbor);

            inFrontier[neighbor] = true;
        }
    }

    // NO PATH FOUND
    result.found = false;

    return result;
}
