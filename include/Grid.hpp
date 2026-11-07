
#pragma once

#include "Cell.hpp"
#include "GridConfig.hpp"

#include <vector>

class Grid
{
public:

    GridConfig config;

    std::vector<Cell> cells;

    // Constructors
    
    Grid();

    explicit Grid(const GridConfig& config);

    // Coordinate / Index conversion

    int getIndex(const Coordinate& position) const;

    Coordinate getCoordinate(int index) const;

    bool isValid(const Coordinate& position) const;

    bool isValidIndex(int index) const;

    // Cell Accesss

    Cell& getCell(const Coordinate& position);

    const Cell& getCell(const Coordinate& position) const;

    Cell& getCell(int x, int y, int z = 0);

    const Cell& getCell(int x, int y, int z = 0) const;

    Cell& getCellById(int id);

    const Cell& getCellById(int id) const;

    // Neighbours

    void updateNeighbors();

    std::vector<int> getNeighbors(int id) const;

    // Barrier Management

    bool isBarrier(int id) const;

    void addBarrier(int id);

    void removeBarrier(int id);

    // Grid Management

    void reset();

    int getSize() const;

    // Dimension Information

    bool is2D() const;

    bool is3D() const;
};
