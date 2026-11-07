
#include "Grid.hpp"

#include <stdexcept>


Grid::Grid()
    : Grid(GridConfig{})
{
}

Grid::Grid(const GridConfig& config)
    : config(config)
{
    if (
        this->config.width <= 0 ||
        this->config.height <= 0 ||
        this->config.depth <= 0
    )
    {
        throw std::invalid_argument(
            "Grid dimensions must be positive."
        );
    }

    cells.reserve(this->config.getTotalCells());

    for (int z = 0; z < this->config.depth; ++z)
    {
        for (int y = 0; y < this->config.height; ++y)
        {
            for (int x = 0; x < this->config.width; ++x)
            {
                cells.emplace_back(x, y, z);
            }
        }
    }

    updateNeighbors();
}

// Coordinate / Index conversion

int Grid::getIndex(const Coordinate& position) const
{
    return
        position.x +
        position.y * config.width +
        position.z * config.width * config.height;
}

Coordinate Grid::getCoordinate(int index) const
{
    if (!isValidIndex(index))
    {
        throw std::out_of_range(
            "Invalid grid cell index."
        );
    }

    Coordinate position;

    position.x = index % config.width;

    position.y =
        (index / config.width) % config.height;

    position.z =
        index / (config.width * config.height);

    return position;
}

bool Grid::isValid(const Coordinate& position) const
{
    return
        position.x >= 0 &&
        position.x < config.width &&

        position.y >= 0 &&
        position.y < config.height &&

        position.z >= 0 &&
        position.z < config.depth;
}

bool Grid::isValidIndex(int index) const
{
    return index >= 0 && index < getSize();
}


// Cell access

Cell& Grid::getCell(const Coordinate& position)
{
    if (!isValid(position))
    {
        throw std::out_of_range(
            "Invalid grid coordinate."
        );
    }

    return cells[getIndex(position)];
}

const Cell& Grid::getCell(
    const Coordinate& position
) const
{
    if (!isValid(position))
    {
        throw std::out_of_range(
            "Invalid grid coordinate."
        );
    }

    return cells[getIndex(position)];
}

Cell& Grid::getCell(int x, int y, int z)
{
    return getCell(Coordinate(x, y, z));
}

const Cell& Grid::getCell(int x, int y, int z) const
{
    return getCell(Coordinate(x, y, z));
}

Cell& Grid::getCellById(int id)
{
    if (!isValidIndex(id))
    {
        throw std::out_of_range(
            "Invalid grid cell ID."
        );
    }

    return cells[id];
}

const Cell& Grid::getCellById(int id) const
{
    if (!isValidIndex(id))
    {
        throw std::out_of_range(
            "Invalid grid cell ID."
        );
    }

    return cells[id];
}

// Neighbours management

std::vector<int> Grid::getNeighbors(int id) const
{
    std::vector<int> neighbors;

    if (!isValidIndex(id))
    {
        return neighbors;
    }

    const Coordinate current = getCoordinate(id);

    // Six possible directions.
    const Coordinate directions[6] = {
        { 1,  0,  0},  // Right
        {-1,  0,  0},  // Left
        { 0,  1,  0},  // Up
        { 0, -1,  0},  // Down
        { 0,  0,  1},  // Forward
        { 0,  0, -1}   // Backward
    };

    // 2D uses four directions.
    // 3D uses all six directions.
    const int directionCount = is2D() ? 4 : 6;

    for (int i = 0; i < directionCount; ++i)
    {
        Coordinate next = current + directions[i];

        if (!isValid(next))
        {
            continue;
        }

        int neighborID = getIndex(next);

        if (cells[neighborID].isBarrier())
        {
            continue;
        }

        neighbors.push_back(neighborID);
    }

    return neighbors;
}

void Grid::updateNeighbors()
{
    for (int id = 0; id < getSize(); ++id)
    {
        cells[id].neighbors = getNeighbors(id);
    }
}

// Barrier Management

bool Grid::isBarrier(int id) const
{
    if (!isValidIndex(id))
    {
        return false;
    }

    return cells[id].isBarrier();
}

void Grid::addBarrier(int id)
{
    if (!isValidIndex(id))
    {
        return;
    }

    cells[id].type = CellType::Barrier;

    updateNeighbors();
}

void Grid::removeBarrier(int id)
{
    if (!isValidIndex(id))
    {
        return;
    }

    if (cells[id].isBarrier())
    {
        cells[id].reset();

        updateNeighbors();
    }
}

void Grid::reset()
{
    for (Cell& cell : cells)
    {
        if (!cell.isBarrier())
        {
            cell.reset();
        }
    }

    updateNeighbors();
}

// Grid Information

int Grid::getSize() const
{
    return static_cast<int>(cells.size());
}

bool Grid::is2D() const
{
    return config.is2D();
}

bool Grid::is3D() const
{
    return config.is3D();
}
