
#include "ComparisonManager.hpp"

#include <algorithm>


ComparisonManager::ComparisonManager() = default;

// Algorithm selection
void ComparisonManager::toggleAlgorithm(
    AlgorithmType algorithm
) {

    auto it = std::find(
        selectedAlgorithms.begin(),
        selectedAlgorithms.end(),
        algorithm
    );

    if (it != selectedAlgorithms.end()) {

        selectedAlgorithms.erase(it);

        // Changing selection invalidates previous results.
        reset();

        return;
    }

    if (selectedAlgorithms.size() >= MAX_ALGORITHMS) {
        return;
    }

    selectedAlgorithms.push_back(algorithm);

    reset();
}

const std::vector<AlgorithmType>&
ComparisonManager::getSelectedAlgorithms() const {

    return selectedAlgorithms;
}

std::string ComparisonManager::getAlgorithmName(
    AlgorithmType algorithm
) const {

    switch (algorithm) {

        case AlgorithmType::DFS:
            return "Depth-First Search";

        case AlgorithmType::BFS:
            return "Breadth-First Search";

        case AlgorithmType::DIJKSTRA:
            return "Dijkstra";

        case AlgorithmType::GREEDY_BEST_FIRST:
            return "Greedy Best-First";

        case AlgorithmType::ASTAR:
            return "A* Search";
    }

    return "Unknown";
}

SearchResult ComparisonManager::executeAlgorithm(
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

        case AlgorithmType::DIJKSTRA:
            return uniformCostSearch(grid, start, end);

        case AlgorithmType::GREEDY_BEST_FIRST:
            return greedyBestFirstSearch(grid, start, end);

        case AlgorithmType::ASTAR:
            return aStarSearch(grid, start, end);
    }

    return SearchResult{};
}

// Calculate algorithm statistics
AlgorithmStatistics ComparisonManager::calculateStatistics(
    const SearchResult& result,
    const Grid& grid
) const {

    AlgorithmStatistics statistics;

    statistics.visitedNodes =
        static_cast<int>(result.exploredOrder.size());

    statistics.pathFound = result.found;

    if (result.found) {

        statistics.pathLength = std::max(
            0,
            static_cast<int>(result.solutionPath.size()) - 1
        );
    }

    // Explored percentage

    int traversableCells = 0;

    for (int i = 0; i < grid.getSize(); i++) {

        if (!grid.isBarrier(i)) {
            traversableCells++;
        }
    }

    if (traversableCells > 0) {

        statistics.exploredPercentage =
            100.0f *
            static_cast<float>(statistics.visitedNodes) /
            static_cast<float>(traversableCells);
    }

    return statistics;
}


void ComparisonManager::start(
    const Grid& grid,
    int start,
    int end
) {

    reset();

    if (selectedAlgorithms.empty()) {
        return;
    }

    if (
        start < 0 ||
        end < 0 ||
        start >= grid.getSize() ||
        end >= grid.getSize() ||
        start == end ||
        grid.isBarrier(start) ||
        grid.isBarrier(end)
    ) {
        return;
    }

    // Each algorithm gets its own SearchResult
    // and its own AlgorithmStatistics.

    for (AlgorithmType algorithm : selectedAlgorithms) {

        AlgorithmView view;

        view.algorithm = algorithm;

        view.name = getAlgorithmName(algorithm);

        view.result = executeAlgorithm(
            algorithm,
            grid,
            start,
            end
        );

        // Calculate statistics immediately after execution.
        view.statistics = calculateStatistics(
            view.result,
            grid
        );

        view.exploredIndex = 0;

        view.pathIndex = 0;

        view.finished = false;

        views.push_back(std::move(view));
    }

    resultsAvailable = true;

    // Handle algorithms with no exploration results.
    for (AlgorithmView& view : views) {

        if (
            view.result.exploredOrder.empty() &&
            !view.result.found
        ) {
            view.finished = true;
        }
    }

    running = !allFinished();

    paused = false;
}

void ComparisonManager::update() {

    if (!running || paused) {
        return;
    }

    for (AlgorithmView& view : views) {

        if (view.finished) {
            continue;
        }

        // --------------------------------------------------
        // REVEAL EXPLORED CELLS
        // --------------------------------------------------

        for (int i = 0; i < stepsPerFrame; ++i) {

            if (
                view.exploredIndex <
                view.result.exploredOrder.size()
            ) {
                ++view.exploredIndex;
            }
            else {
                break;
            }
        }

        // --------------------------------------------------
        // REVEAL SOLUTION PATH
        // --------------------------------------------------

        if (
            view.exploredIndex >=
            view.result.exploredOrder.size()
        ) {

            if (view.result.found) {

                for (int i = 0; i < stepsPerFrame; ++i) {

                    if (
                        view.pathIndex <
                        view.result.solutionPath.size()
                    ) {
                        ++view.pathIndex;
                    }
                    else {
                        break;
                    }
                }

                if (
                    view.pathIndex >=
                    view.result.solutionPath.size()
                ) {
                    view.finished = true;
                }

            }
            else {

                view.finished = true;
            }
        }
    }

    if (allFinished()) {

        running = false;

        paused = false;
    }
}

// Playback controls

void ComparisonManager::pause() {

    if (running) {
        paused = true;
    }
}


void ComparisonManager::resume() {

    if (running && paused) {
        paused = false;
    }
}


void ComparisonManager::reset() {

    views.clear();

    running = false;

    paused = false;

    resultsAvailable = false;
}

// Status

bool ComparisonManager::isRunning() const {

    return running;
}


bool ComparisonManager::isPaused() const {

    return paused;
}


bool ComparisonManager::hasResults() const {

    return resultsAvailable;
}

void ComparisonManager::setAnimationSpeed(int steps) {

    stepsPerFrame = std::max(1, steps);
}

const std::vector<AlgorithmView>&
ComparisonManager::getViews() const {

    return views;
}

// Check completion

bool ComparisonManager::allFinished() const {

    if (views.empty()) {
        return true;
    }

    for (const AlgorithmView& view : views) {

        if (!view.finished) {
            return false;
        }
    }

    return true;
}
