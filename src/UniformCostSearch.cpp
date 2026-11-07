
#include "Pathfinding.hpp"
#include "Grid.hpp"
#include "Cell.hpp"

#include <vector>
#include <queue>
#include <tuple>
#include <functional>
#include <algorithm>
#include <limits>

// ============================================================
// UNIFORM-COST SEARCH (DIJKSTRA)
//
// Frontier: Priority Queue
// Evaluation: Accumulated path cost g(n)
// Movement: 4 neighbours in 2D, 6 neighbours in 3D
// Guarantees: Lowest-cost path for non-negative edge costs
// ============================================================


// Search
SearchResult uniformCostSearch(
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

    // Priority Queue
    // Each entry contains:
    // 1. Accumulated path cost
    // 2. Insertion counter (FIFO tie-breaking)
    // 3. Cell index
    //
    // The priority queue sorts ascending by path cost.
    // The insertion counter preserves FIFO ordering
    // when multiple cells have identical path costs.

    using QueueElement = std::tuple<int, int, int>;

    std::priority_queue<
        QueueElement,
        std::vector<QueueElement>,
        std::greater<QueueElement>
    > frontier;

    int counter = 0;

    // Initialize Data Structures

    const int INF = std::numeric_limits<int>::max();

    std::vector<int> distance(totalCells, INF);

    std::vector<int> parent(totalCells, -1);

    std::vector<bool> explored(totalCells, false);

    distance[start] = 0;

    frontier.push({
        0,
        counter++,
        start
    });

    // Main search loop
    while (!frontier.empty())
    {
        // Retrieve cell with lowest accumulated path cost.
        auto [currentCost, insertionOrder, current] =
            frontier.top();

        frontier.pop();

        // Ignore outdated queue entries.
        if (currentCost != distance[current])
        {
            continue;
        }

        // Ignore already explored cells.
        if (explored[current])
        {
            continue;
        }

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

        // Explore current cell

        explored[current] = true;

        // Record exploration order for visualization.
        result.exploredOrder.push_back(current);

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

            // Skip already explored cells.
            if (explored[neighbor])
            {
                continue;
            }

            // Every movement currently has a cost of 1.
            const int newCost = currentCost + 1;

            // Update only if a cheaper path was found.
            if (newCost < distance[neighbor])
            {
                distance[neighbor] = newCost;

                parent[neighbor] = current;

                // Insert updated entry into priority queue.
                frontier.push({
                    newCost,
                    counter++,
                    neighbor
                });
            }
        }
    }

    // NO PATH FOUND
    result.found = false;

    return result;
}
