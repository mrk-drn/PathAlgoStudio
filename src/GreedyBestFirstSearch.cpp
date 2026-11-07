
#include "Pathfinding.hpp"
#include "Grid.hpp"
#include "Cell.hpp"

#include <vector>
#include <queue>
#include <tuple>
#include <functional>
#include <algorithm>

// ============================================================
// GREEDY BEST-FIRST SEARCH (GRAPH SEARCH)
//
// Frontier: Priority Queue
// Evaluation: h(n) = Manhattan distance to goal
// Movement: 4 neighbours in 2D, 6 neighbours in 3D
// ============================================================


// Manhattan Heuristic
static int heuristic(
    const Coordinate& a,
    const Coordinate& b
)
{
    return a.manhattanDistance(b);
}


// Search
SearchResult greedyBestFirstSearch(
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
    // 1. Heuristic value
    // 2. Insertion counter (FIFO tie-breaking)
    // 3. Cell index

    using QueueElement = std::tuple<int, int, int>;

    std::priority_queue<
        QueueElement,
        std::vector<QueueElement>,
        std::greater<QueueElement>
    > frontier;

    int counter = 0;

    // Initialize data structures

    std::vector<bool> explored(totalCells, false);

    std::vector<bool> inFrontier(totalCells, false);

    std::vector<int> parent(totalCells, -1);

    // Initialize coordinates

    const Coordinate startPosition =
        grid.getCoordinate(start);

    const Coordinate endPosition =
        grid.getCoordinate(end);

    // Calculate heuristic for start.
    const int startHeuristic = heuristic(
        startPosition,
        endPosition
    );

    // Insert start into frontier.
    frontier.push({
        startHeuristic,
        counter++,
        start
    });

    inFrontier[start] = true;

    // Main search loop
    while (!frontier.empty())
    {
        // Retrieve cell with lowest heuristic value.
        auto [currentHeuristic, insertionOrder, current] =
            frontier.top();

        frontier.pop();

        inFrontier[current] = false;

        // Skip already explored cells.
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

            // Skip explored or already queued cells.
            if (
                explored[neighbor] ||
                inFrontier[neighbor]
            )
            {
                continue;
            }

            // Get neighbour coordinates.
            const Coordinate neighborPosition =
                grid.getCoordinate(neighbor);

            // Calculate heuristic distance to goal.
            const int neighborHeuristic = heuristic(
                neighborPosition,
                endPosition
            );

            // Record parent.
            parent[neighbor] = current;

            // Insert neighbour into priority queue.
            frontier.push({
                neighborHeuristic,
                counter++,
                neighbor
            });

            inFrontier[neighbor] = true;
        }
    }

    // No path found
    result.found = false;

    return result;
}
