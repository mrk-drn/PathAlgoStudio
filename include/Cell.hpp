
#pragma once

#include "Coordinate.hpp"

#include <vector>


enum class CellType
{
    Empty,
    Start,
    End,
    Barrier,
    Open,
    Closed,
    Path
};

struct Cell
{
    // Cell position in 2D or 3D space.
    Coordinate position;

    // Cell state.
    CellType type = CellType::Empty;

    // IDs of neighbouring cells.
    std::vector<int> neighbors;

    // Constructor
    Cell(int x, int y, int z = 0)
        : position(x, y, z)
    {
    }

    explicit Cell(const Coordinate& position)
        : position(position)
    {
    }

    const Coordinate& getPosition() const
    {
        return position;
    }

    // Cell State
    bool isBarrier() const
    {
        return type == CellType::Barrier;
    }

    void reset()
    {
        type = CellType::Empty;
    }
};
