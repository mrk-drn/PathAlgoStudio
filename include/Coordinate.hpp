#pragma once

#include <cstdlib>


struct Coordinate
{
    int x = 0;
    int y = 0;
    int z = 0;

    Coordinate() = default;

    Coordinate(int x, int y, int z = 0)
        : x(x), y(y), z(z)
    {
    }

    // Comparison
    bool operator==(const Coordinate& other) const
    {
        return
            x == other.x &&
            y == other.y &&
            z == other.z;
    }

    bool operator!=(const Coordinate& other) const
    {
        return !(*this == other);
    }

    // Coordinate addition
    Coordinate operator+(const Coordinate& other) const
    {
        return {
            x + other.x,
            y + other.y,
            z + other.z
        };
    }

    Coordinate operator-(const Coordinate& other) const
    {
        return {
            x - other.x,
            y - other.y,
            z - other.z
        };
    }

    int manhattanDistance(const Coordinate& other) const
    {
        return
            std::abs(x - other.x) +
            std::abs(y - other.y) +
            std::abs(z - other.z);
    }
};
