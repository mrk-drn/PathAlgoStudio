
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
// A* SEARCH
//
// Frontier: Priority Queue
// Evaluation: f(n) = g(n) + h(n)
// Movement: 4 neighbours in 2D, 6 neighbours in 3D
// ============================================================


static int heuristic(
    const Coordinate& a,
    const Coordinate& b
)
{
    return a.manhattanDistance(b);
}

SearchResult aStarSearch(
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

    using QueueElement = std::tuple<int, int, int>;

    std::priority_queue<
        QueueElement,
        std::vector<QueueElement>,
        std::greater<QueueElement>
    > frontier;

    int counter = 0;

    // Data Structures

    const int INF = std::numeric_limits<int>::max();

    std::vector<int> pathCosts(totalCells, INF);

    std::vector<int> parent(totalCells, -1);

    std::vector<bool> explored(totalCells, false);

    // Initialization

    const Coordinate startPosition =
        grid.getCoordinate(start);

    const Coordinate endPosition =
        grid.getCoordinate(end);

    pathCosts[start] = 0;

    const int startHeuristic = heuristic(
        startPosition,
        endPosition
    );

    frontier.push({
        startHeuristic,
        counter++,
        start
    });

    // Main search loop

    while (!frontier.empty())
    {
        auto [currentEvaluation, insertionOrder, current] =
            frontier.top();

        frontier.pop();

        // Ignore already explored cells.
        if (explored[current])
        {
            continue;
        }

        // Goal reached

        if (current == end)
        {
            result.found = true;

            int currentPathCell = end;

            while (currentPathCell != -1)
            {
                result.solutionPath.push_back(
                    currentPathCell
                );

                currentPathCell = parent[currentPathCell];
            }

            std::reverse(
                result.solutionPath.begin(),
                result.solutionPath.end()
            );

            return result;
        }

        // Explore current cell

        explored[current] = true;

        result.exploredOrder.push_back(current);

        // Explore Neighbours

        const std::vector<int> neighbors =
            grid.getNeighbors(current);

        for (int neighbor : neighbors)
        {
            if (explored[neighbor])
            {
                continue;
            }

            // Every movement costs 1.
            const int newPathCost =
                pathCosts[current] + 1;

            if (newPathCost < pathCosts[neighbor])
            {
                pathCosts[neighbor] = newPathCost;

                parent[neighbor] = current;

                const Coordinate neighborPosition =
                    grid.getCoordinate(neighbor);

                const int h = heuristic(
                    neighborPosition,
                    endPosition
                );

                const int evaluationValue =
                    newPathCost + h;

                frontier.push({
                    evaluationValue,
                    counter++,
                    neighbor
                });
            }
        }
    }

    // No Path found
    result.found = false;

    return result;
}
