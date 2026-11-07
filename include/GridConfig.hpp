#pragma once


enum class Dimension
{
    TWO_D,
    THREE_D
};

struct GridConfig
{
    Dimension dimension = Dimension::TWO_D;

    int width = 30;
    int height = 30;
    int depth = 1;

    GridConfig() = default;

    GridConfig(
        Dimension dimension,
        int width,
        int height,
        int depth = 1
    )
        : dimension(dimension),
          width(width),
          height(height),
          depth(dimension == Dimension::TWO_D ? 1 : depth)
    {
    }

    // Total number of cells

    int getTotalCells() const
    {
        return width * height * depth;
    }

    // Dimension checks

    bool is2D() const
    {
        return dimension == Dimension::TWO_D;
    }

    bool is3D() const
    {
        return dimension == Dimension::THREE_D;
    }
};
