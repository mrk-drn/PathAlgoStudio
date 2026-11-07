
#include "AlgorithmVisualizer.hpp"

#include <algorithm>

// ============================================================
// CONSTRUCTOR
// ============================================================

AlgorithmVisualizer::AlgorithmVisualizer(
    const AlgorithmView& view
)
{
    initialize(view);
}

// ============================================================
// INITIALIZATION
// ============================================================

void AlgorithmVisualizer::initialize(
    const AlgorithmView& view
)
{
    algorithmName = view.name;

    visibleExploredCells.clear();
    visiblePathCells.clear();

    finished = false;
    explorationComplete = false;
}

// ============================================================
// UPDATE
// ============================================================

void AlgorithmVisualizer::update(
    const AlgorithmView& view
)
{
    // Safeguard against indices exceeding vector sizes.
    const size_t exploredLimit = std::min(
        view.exploredIndex,
        view.result.exploredOrder.size()
    );

    const size_t pathLimit = std::min(
        view.pathIndex,
        view.result.solutionPath.size()
    );

    // Append newly visible explored cells.
    while (
        visibleExploredCells.size() < exploredLimit
    )
    {
        visibleExploredCells.push_back(
            view.result.exploredOrder[
                visibleExploredCells.size()
            ]
        );
    }

    // Append newly visible solution-path cells.
    while (
        visiblePathCells.size() < pathLimit
    )
    {
        visiblePathCells.push_back(
            view.result.solutionPath[
                visiblePathCells.size()
            ]
        );
    }

    // Detect when exploration has completely finished.
    explorationComplete =
        view.exploredIndex >=
        view.result.exploredOrder.size();

    // Update overall completion state.
    finished = view.finished;
}

// ============================================================
// RESET
// ============================================================

void AlgorithmVisualizer::reset()
{
    visibleExploredCells.clear();
    visiblePathCells.clear();

    finished = false;
    explorationComplete = false;
}

// ============================================================
// GETTERS
// ============================================================

const std::vector<int>&
AlgorithmVisualizer::getVisibleExploredCells() const
{
    return visibleExploredCells;
}

const std::vector<int>&
AlgorithmVisualizer::getVisiblePathCells() const
{
    return visiblePathCells;
}

bool AlgorithmVisualizer::isFinished() const
{
    return finished;
}

bool AlgorithmVisualizer::isExplorationComplete() const
{
    return explorationComplete;
}

const std::string&
AlgorithmVisualizer::getName() const
{
    return algorithmName;
}
