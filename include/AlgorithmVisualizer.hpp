
#pragma once

#include "ComparisonManager.hpp"

#include <cstddef>
#include <vector>
#include <string>

class AlgorithmVisualizer {

public:

    AlgorithmVisualizer() = default;

    explicit AlgorithmVisualizer(const AlgorithmView& view);

    // Initialize visualization from an algorithm result.
    void initialize(const AlgorithmView& view);

    // Update animation state.
    void update(const AlgorithmView& view);

    // Reset visualization.
    void reset();

    // Retrieve explored cells currently visible.
    const std::vector<int>& getVisibleExploredCells() const;

    // Retrieve solution path currently visible.
    const std::vector<int>& getVisiblePathCells() const;

    // Check whether visualization is finished.
    bool isFinished() const;

    // Check whether exploration is complete.
    bool isExplorationComplete() const;

    // Get algorithm name.
    const std::string& getName() const;

private:

    std::string algorithmName;

    std::vector<int> visibleExploredCells;

    std::vector<int> visiblePathCells;

    bool finished = false;

    bool explorationComplete = false;
};
