
#pragma once

#include "Pathfinding.hpp"
#include "AlgorithmStatistics.hpp"

#include <vector>
#include <string>
#include <cstddef>

struct AlgorithmView {

    AlgorithmType algorithm;

    std::string name;

    SearchResult result;

    // Algorithm statistics
    AlgorithmStatistics statistics;

    size_t exploredIndex = 0;
    size_t pathIndex = 0;

    bool finished = false;
};

class ComparisonManager {

public:

    static constexpr int MAX_ALGORITHMS = 2;

    ComparisonManager();

    // Algorithm selection
    void toggleAlgorithm(AlgorithmType algorithm);

    const std::vector<AlgorithmType>&
    getSelectedAlgorithms() const;

    // Start comparison
    void start(
        const Grid& grid,
        int start,
        int end
    );

    // Animation update
    void update();

    // Playback controls
    void pause();
    void resume();
    void reset();

    bool isRunning() const;
    bool isPaused() const;
    bool hasResults() const;

    // Animation speed
    void setAnimationSpeed(int stepsPerFrame);

    // Access visualization data
    const std::vector<AlgorithmView>&
    getViews() const;

    bool allFinished() const;

private:

    std::vector<AlgorithmType> selectedAlgorithms;

    std::vector<AlgorithmView> views;

    bool running = false;
    bool paused = false;
    bool resultsAvailable = false;

    int stepsPerFrame = 1;

    // Execute algorithm
    SearchResult executeAlgorithm(
        AlgorithmType algorithm,
        const Grid& grid,
        int start,
        int end
    );

    // Get algorithm name
    std::string getAlgorithmName(
        AlgorithmType algorithm
    ) const;

    // Calculate algorithm statistics
    AlgorithmStatistics calculateStatistics(
        const SearchResult& result,
        const Grid& grid
    ) const;
};
