
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include "Grid.hpp"
#include "Cell.hpp"
#include "Coordinate.hpp"
#include "GridConfig.hpp"
#include "Pathfinding.hpp"
#include "ComparisonManager.hpp"
#include "AlgorithmVisualizer.hpp"
#include "AlgorithmStatistics.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>


// Window configuration

int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 800;
const int CONTROL_PANEL_WIDTH = 300;
const int VIEWPORT_DIVIDER_WIDTH = 2;
float controlPanelScroll = 0.0f;
const float CONTROL_PANEL_CONTENT_HEIGHT = 990.0f;
const int SCROLLBAR_WIDTH = 8;

// Grid configuration

const float CELL_SIZE = 1.0f;
const float GRID_SPACING = 5.0f;

const int DEFAULT_GRID_WIDTH = 10;
const int DEFAULT_GRID_HEIGHT = 10;
const int DEFAULT_GRID_DEPTH = 10;

// Camera configuration

const float SINGLE_CAMERA_DISTANCE = 55.0f;
const float DUAL_CAMERA_DISTANCE = 85.0f;

const float MIN_CAMERA_DISTANCE = 8.0f;
const float MAX_CAMERA_DISTANCE = 300.0f;

const float CAMERA_NEAR_PLANE = 0.01f;
const float CAMERA_FAR_PLANE = 1000.0f;

const float CAMERA_ROTATION_SENSITIVITY = 0.0025f;
const float CAMERA_PAN_SENSITIVITY = 0.0025f;

const float CAMERA_MIN_PITCH = -1.45f;
const float CAMERA_MAX_PITCH = 1.45f;

// Speed configuration

float visualizationSpeed = 8.0f;

float visualizationAccumulator = 0.0f;

const float MIN_VISUALIZATION_SPEED = 1.0f;
const float MAX_VISUALIZATION_SPEED = 30.0f;

// Colours

const Color BACKGROUND_COLOR = {
    24, 29, 39, 255
};

const Color DEFAULT_COLOR = {
    0, 0, 0, 0
};

const Color START_COLOR = {
    40, 100, 255, 255
};

const Color END_COLOR = {
    230, 55, 55, 255
};

const Color BARRIER_COLOR = {
    0, 0, 0, 255
};

// const Color EXPLORED_COLOR = {
//     50, 160, 255, 255
// };
const Color EXPLORED_COLOR = {
    255, 255, 255, 255
};

const Color CURRENT_EXPLORED_COLOR = {
    255, 255, 0, 255
};

// const Color PATH_COLOR = {
//     255, 215, 0, 255
// };
const Color PATH_COLOR = {
    50, 205, 50, 255
};

const Color BUTTON_COLOR = {
    45, 55, 75, 255
};

const Color BUTTON_HOVER_COLOR = {
    65, 100, 150, 255
};

const Color BUTTON_SELECTED_COLOR = {
    40, 130, 100, 255
};

const Color TEXT_COLOR = RAYWHITE;


// Global application state

Grid grid;

ComparisonManager comparisonManager;

std::vector<AlgorithmVisualizer> visualizers;

int startIndex = -1;
int endIndex = -1;

int lastPaintedCell = -1;

Dimension currentDimension = Dimension::TWO_D;

// Edit mode

enum class EditMode
{
    NONE,
    START,
    END,
    BARRIER
};

EditMode editMode = EditMode::NONE;

// Camera state

Camera3D leftCamera = { 0 };
Camera3D rightCamera = { 0 };

bool leftCameraDragging = false;
bool rightCameraDragging = false;

bool leftCameraPanning = false;
bool rightCameraPanning = false;

bool comparisonModeActive = false;

// Viewport state

int leftViewportX = CONTROL_PANEL_WIDTH;
int rightViewportX = CONTROL_PANEL_WIDTH;

int leftViewportWidth =
    SCREEN_WIDTH - CONTROL_PANEL_WIDTH;

int rightViewportWidth =
    SCREEN_WIDTH - CONTROL_PANEL_WIDTH;

// Grid coordinate helpers

Vector3 gridToWorld(const Coordinate& position)
{
    if (grid.is2D())
    {
        return {
            position.x * GRID_SPACING,
            0.0f,
            position.y * GRID_SPACING
        };
    }

    return {
        position.x * GRID_SPACING,
        position.y * GRID_SPACING,
        position.z * GRID_SPACING
    };
}

Vector3 getGridCenter()
{
    const GridConfig& config = grid.config;

    if (grid.is2D())
    {
        return {
            (config.width - 1) * GRID_SPACING * 0.5f,
            0.0f,
            (config.height - 1) * GRID_SPACING * 0.5f
        };
    }

    return {
        (config.width - 1) * GRID_SPACING * 0.5f,
        (config.height - 1) * GRID_SPACING * 0.5f,
        (config.depth - 1) * GRID_SPACING * 0.5f
    };
}

float getGridWorldSize()
{
    const GridConfig& config = grid.config;

    const float width =
        config.width * GRID_SPACING;

    const float height =
        config.height * GRID_SPACING;

    const float depth =
        config.depth * GRID_SPACING;

    return std::max(
        width,
        std::max(height, depth)
    );
}

// Camera management

void initializeCamera(Camera3D& camera)
{
    const Vector3 center = getGridCenter();

    const float distance =
        getGridWorldSize() * 1.5f;

    camera.target = center;

    if (grid.is2D())
    {
        camera.position = {
            center.x,
            distance * 0.65f,
            center.z + distance
        };
    }
    else
    {
        camera.position = {
            center.x + distance * 0.65f,
            center.y + distance * 0.55f,
            center.z + distance
        };
    }

    camera.up = {
        0.0f,
        1.0f,
        0.0f
    };

    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void setCameraDistance(
    Camera3D& camera,
    float distance
)
{
    Vector3 direction = Vector3Subtract(
        camera.position,
        camera.target
    );

    direction = Vector3Normalize(direction);

    camera.position = Vector3Add(
        camera.target,
        Vector3Scale(direction, distance)
    );
}

void configureSingleCamera()
{
    initializeCamera(leftCamera);

    setCameraDistance(
        leftCamera,
        std::max(
            SINGLE_CAMERA_DISTANCE,
            getGridWorldSize() * 1.2f
        )
    );

    leftCameraDragging = false;
    leftCameraPanning = false;
}

void configureComparisonCameras()
{
    initializeCamera(leftCamera);
    initializeCamera(rightCamera);

    const float distance = std::max(
        DUAL_CAMERA_DISTANCE,
        getGridWorldSize() * 1.5f
    );

    setCameraDistance(leftCamera, distance);
    setCameraDistance(rightCamera, distance);

    leftCamera.fovy = 45.0f;
    rightCamera.fovy = 45.0f;

    leftCameraDragging = false;
    rightCameraDragging = false;

    leftCameraPanning = false;
    rightCameraPanning = false;
}

void resetCamera()
{
    if (comparisonModeActive && visualizers.size() == 2)
    {
        configureComparisonCameras();
    }
    else
    {
        configureSingleCamera();
    }
}

// VIEWPORT CONFIGURATION

void updateViewportConfiguration()
{
    const int screenWidth = GetScreenWidth();

    const int availableWidth =
        screenWidth - CONTROL_PANEL_WIDTH;

    const bool comparingTwo =
        comparisonModeActive &&
        visualizers.size() == 2;

    leftViewportX = CONTROL_PANEL_WIDTH;

    if (comparingTwo)
    {
        const int halfWidth = availableWidth / 2;

        leftViewportWidth = halfWidth;

        rightViewportX =
            CONTROL_PANEL_WIDTH + halfWidth;

        rightViewportWidth =
            availableWidth - halfWidth;
    }
    else
    {
        leftViewportWidth = availableWidth;

        rightViewportX = screenWidth;

        rightViewportWidth = 0;
    }
}

// VIEWPORT-AWARE 3D RENDERING

void beginViewport3D(
    Camera3D& camera,
    int viewportX,
    int viewportWidth
)
{
    if (viewportWidth <= 0)
        return;

    const int screenHeight = GetScreenHeight();

    BeginScissorMode(
        viewportX,
        0,
        viewportWidth,
        screenHeight
    );

    rlDrawRenderBatchActive();

    rlViewport(
        viewportX,
        0,
        viewportWidth,
        screenHeight
    );

    BeginMode3D(camera);

    rlDrawRenderBatchActive();

    const float aspectRatio =
        static_cast<float>(viewportWidth) /
        static_cast<float>(screenHeight);

    Matrix projection = MatrixPerspective(
        camera.fovy * DEG2RAD,
        aspectRatio,
        CAMERA_NEAR_PLANE,
        CAMERA_FAR_PLANE
    );

    rlMatrixMode(RL_PROJECTION);
    rlLoadIdentity();
    rlMultMatrixf(MatrixToFloat(projection));

    rlMatrixMode(RL_MODELVIEW);
}

// END VIEWPORT RENDERING

void endViewport3D()
{
    rlDrawRenderBatchActive();

    EndMode3D();

    rlViewport(
        0,
        0,
        GetScreenWidth(),
        GetScreenHeight()
    );

    EndScissorMode();
}


// Cell and Grid management

Color getCellColor(
    int cellIndex,
    const AlgorithmVisualizer& visualizer
)
{
    // Start cell.
    if (cellIndex == startIndex)
        return START_COLOR;

    // Goal cell.
    if (cellIndex == endIndex)
        return END_COLOR;

    // Barrier.
    if (grid.getCellById(cellIndex).isBarrier())
        return BARRIER_COLOR;

    const auto& explored =
        visualizer.getVisibleExploredCells();

    const auto& path =
        visualizer.getVisiblePathCells();

    // Final solution path takes priority.
    if (
        std::find(
            path.begin(),
            path.end(),
            cellIndex
        ) != path.end()
    )
    {
        return PATH_COLOR;
    }

    // Currently explored cell is yellow,
    // but only while exploration is ongoing.
    if (
        !explored.empty() &&
        explored.back() == cellIndex &&
        !visualizer.isExplorationComplete()
    )
    {
        return CURRENT_EXPLORED_COLOR;
    }

    // Previously explored cells are white.
    if (
        std::find(
            explored.begin(),
            explored.end(),
            cellIndex
        ) != explored.end()
    )
    {
        return EXPLORED_COLOR;
    }

    // Unvisited cell.
    return DEFAULT_COLOR;
}

void drawGridCell(
    int cellIndex,
    Color color
)
{
    const Cell& cell =
        grid.getCellById(cellIndex);

    const Vector3 position =
        gridToWorld(cell.position);

    // Determine whether this cell should be filled.
    const bool visible =
        color.a > 0;

    if (grid.is2D())
    {
        // Draw filled cell only if it is visible.
        if (visible)
        {
            DrawCube(
                position,
                CELL_SIZE,
                0.15f,
                CELL_SIZE,
                color
            );
        }

        // Always draw the cell outline.
        DrawCubeWires(
            position,
            CELL_SIZE,
            0.15f,
            CELL_SIZE,
            DARKGRAY
        );
    }
    else
    {
        // Draw filled cube only if it is visible.
        if (visible)
        {
            DrawCube(
                position,
                CELL_SIZE,
                CELL_SIZE,
                CELL_SIZE,
                color
            );
        }

        // Always draw the cube outline.
        DrawCubeWires(
            position,
            CELL_SIZE,
            CELL_SIZE,
            CELL_SIZE,
            DARKGRAY
        );
    }
}

void drawGrid(
    const AlgorithmVisualizer& visualizer
)
{
    for (int i = 0; i < grid.getSize(); i++)
    {
        drawGridCell(
            i,
            getCellColor(i, visualizer)
        );
    }
}

void drawEditableGrid()
{
    for (int i = 0; i < grid.getSize(); i++)
    {
        Color color = DEFAULT_COLOR;

        if (i == startIndex)
        {
            color = START_COLOR;
        }
        else if (i == endIndex)
        {
            color = END_COLOR;
        }
        else if (grid.getCellById(i).isBarrier())
        {
            color = BARRIER_COLOR;
        }

        drawGridCell(i, color);
    }
}

// 3D ray-to-voxel intersection

int getSelectedVoxel(const Ray& ray)
{
    const float halfSize = CELL_SIZE * 0.5f;

    float closestDistance =
        std::numeric_limits<float>::max();

    int selectedCell = -1;

    for (int i = 0; i < grid.getSize(); i++)
    {
        const Cell& cell = grid.getCellById(i);

        Vector3 center =
            gridToWorld(cell.position);

        BoundingBox box = {
            {
                center.x - halfSize,
                center.y - halfSize,
                center.z - halfSize
            },
            {
                center.x + halfSize,
                center.y + halfSize,
                center.z + halfSize
            }
        };

        RayCollision collision =
            GetRayCollisionBox(ray, box);

        if (
            collision.hit &&
            collision.distance < closestDistance
        )
        {
            closestDistance = collision.distance;
            selectedCell = i;
        }
    }

    return selectedCell;
}

int getSelectedCell2D(
    Camera3D& camera,
    int viewportX,
    int viewportWidth
)
{
    Vector2 mouse = GetMousePosition();

    if (
        mouse.x < viewportX ||
        mouse.x >= viewportX + viewportWidth ||
        mouse.y < 0 ||
        mouse.y >= SCREEN_HEIGHT
    )
    {
        return -1;
    }

    Vector2 localMouse = {
        mouse.x - viewportX,
        mouse.y
    };

    Ray ray = GetScreenToWorldRayEx(
        localMouse,
        camera,
        viewportWidth,
        SCREEN_HEIGHT
    );

    if (fabsf(ray.direction.y) < 0.0001f)
        return -1;

    float distance =
        -ray.position.y / ray.direction.y;

    if (distance < 0.0f)
        return -1;

    Vector3 hit = Vector3Add(
        ray.position,
        Vector3Scale(ray.direction, distance)
    );

    int x = static_cast<int>(
        roundf(hit.x / GRID_SPACING)
    );

    int y = static_cast<int>(
        roundf(hit.z / GRID_SPACING)
    );

    if (
        x < 0 ||
        x >= grid.config.width ||
        y < 0 ||
        y >= grid.config.height
    )
    {
        return -1;
    }

    return x + y * grid.config.width;
}

int getSelectedCell3D(
    Camera3D& camera,
    int viewportX,
    int viewportWidth
)
{
    Vector2 mouse = GetMousePosition();

    if (
        mouse.x < viewportX ||
        mouse.x >= viewportX + viewportWidth ||
        mouse.y < 0 ||
        mouse.y >= SCREEN_HEIGHT
    )
    {
        return -1;
    }

    Vector2 localMouse = {
        mouse.x - viewportX,
        mouse.y
    };

    Ray ray = GetScreenToWorldRayEx(
        localMouse,
        camera,
        viewportWidth,
        SCREEN_HEIGHT
    );

    return getSelectedVoxel(ray);
}

int getSelectedGridCell(
    Camera3D& camera,
    int viewportX,
    int viewportWidth
)
{
    if (grid.is2D())
    {
        return getSelectedCell2D(
            camera,
            viewportX,
            viewportWidth
        );
    }

    return getSelectedCell3D(
        camera,
        viewportX,
        viewportWidth
    );
}

void editCell(int cellIndex)
{
    if (cellIndex < 0)
        return;

    if (comparisonManager.isRunning())
        return;

    switch (editMode)
    {
        case EditMode::START:
        {
            if (cellIndex == endIndex)
                return;

            startIndex = cellIndex;

            grid.getCellById(cellIndex).type =
                CellType::Empty;

            grid.updateNeighbors();

            break;
        }

        case EditMode::END:
        {
            if (cellIndex == startIndex)
                return;

            endIndex = cellIndex;

            grid.getCellById(cellIndex).type =
                CellType::Empty;

            grid.updateNeighbors();

            break;
        }

        case EditMode::BARRIER:
        {
            if (
                cellIndex == startIndex ||
                cellIndex == endIndex
            )
            {
                return;
            }

            if (grid.getCellById(cellIndex).isBarrier())
            {
                grid.getCellById(cellIndex).type =
                    CellType::Empty;
            }
            else
            {
                grid.getCellById(cellIndex).type =
                    CellType::Barrier;
            }

            grid.updateNeighbors();

            break;
        }

        case EditMode::NONE:
        default:
            break;
    }
}


// Camera update

void updateCamera(
    Camera3D& camera,
    bool& isRotating,
    bool& isPanning,
    int viewportX,
    int viewportWidth
)
{
    if (viewportWidth <= 0)
        return;

    Vector2 mouse = GetMousePosition();

    const bool mouseInsideViewport =
        mouse.x >= viewportX &&
        mouse.x < viewportX + viewportWidth &&
        mouse.y >= 0 &&
        mouse.y < SCREEN_HEIGHT;

    const bool controlPressed =
        IsKeyDown(KEY_LEFT_CONTROL) ||
        IsKeyDown(KEY_RIGHT_CONTROL);

    // zoom
    if (mouseInsideViewport)
    {
        float wheel = GetMouseWheelMove();

        if (wheel != 0.0f)
        {
            Vector3 offset = Vector3Subtract(
                camera.position,
                camera.target
            );

            float distance = Vector3Length(offset);

            distance *=
                (wheel > 0.0f ? 0.85f : 1.18f);

            distance = Clamp(
                distance,
                MIN_CAMERA_DISTANCE,
                MAX_CAMERA_DISTANCE
            );

            offset = Vector3Scale(
                Vector3Normalize(offset),
                distance
            );

            camera.position = Vector3Add(
                camera.target,
                offset
            );
        }
    }

    // Left mouse: orbit
    
    if (
        mouseInsideViewport &&
        !controlPressed &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
    )
    {
        isRotating = true;
    }

    if (
        IsMouseButtonReleased(MOUSE_BUTTON_LEFT) ||
        controlPressed
    )
    {
        isRotating = false;
    }

    if (
        isRotating &&
        IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
        !controlPressed &&
        mouseInsideViewport
    )
    {
        Vector2 delta = GetMouseDelta();

        Vector3 offset = Vector3Subtract(
            camera.position,
            camera.target
        );

        float distance = Vector3Length(offset);

        if (distance > 0.001f)
        {
            float yaw = atan2f(
                offset.x,
                offset.z
            );

            float pitch = asinf(
                Clamp(
                    offset.y / distance,
                    -1.0f,
                    1.0f
                )
            );

            yaw -=
                delta.x * CAMERA_ROTATION_SENSITIVITY;

            pitch +=
                delta.y * CAMERA_ROTATION_SENSITIVITY;

            pitch = Clamp(
                pitch,
                CAMERA_MIN_PITCH,
                CAMERA_MAX_PITCH
            );

            offset.x =
                distance * cosf(pitch) * sinf(yaw);

            offset.y =
                distance * sinf(pitch);

            offset.z =
                distance * cosf(pitch) * cosf(yaw);

            camera.position = Vector3Add(
                camera.target,
                offset
            );
        }
    }

    // Right mouse: pan
    
    if (
        mouseInsideViewport &&
        IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)
    )
    {
        isPanning = true;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
    {
        isPanning = false;
    }

    if (
        isPanning &&
        IsMouseButtonDown(MOUSE_BUTTON_RIGHT) &&
        mouseInsideViewport
    )
    {
        Vector2 delta = GetMouseDelta();

        Vector3 forward = Vector3Normalize(
            Vector3Subtract(
                camera.target,
                camera.position
            )
        );

        Vector3 right = Vector3Normalize(
            Vector3CrossProduct(
                forward,
                camera.up
            )
        );

        Vector3 horizontalForward = {
            forward.x,
            0.0f,
            forward.z
        };

        if (
            Vector3Length(horizontalForward) > 0.0001f
        )
        {
            horizontalForward = Vector3Normalize(
                horizontalForward
            );
        }

        float distance = Vector3Distance(
            camera.position,
            camera.target
        );

        float panSpeed =
            distance * CAMERA_PAN_SENSITIVITY;

        Vector3 movement = Vector3Add(
            Vector3Scale(
                right,
                -delta.x * panSpeed
            ),
            Vector3Scale(
                horizontalForward,
                delta.y * panSpeed
            )
        );

        camera.position = Vector3Add(
            camera.position,
            movement
        );

        camera.target = Vector3Add(
            camera.target,
            movement
        );
    }
}


void updateGridInput(
    Camera3D& camera,
    int viewportX,
    int viewportWidth
)
{
    if (comparisonManager.isRunning())
        return;

    const bool controlPressed =
        IsKeyDown(KEY_LEFT_CONTROL) ||
        IsKeyDown(KEY_RIGHT_CONTROL);

    if (!controlPressed)
    {
        lastPaintedCell = -1;
        return;
    }

    if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        lastPaintedCell = -1;
        return;
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        return;

    const int cellIndex = getSelectedGridCell(
        camera,
        viewportX,
        viewportWidth
    );

    if (cellIndex < 0)
        return;

    if (editMode == EditMode::BARRIER)
    {
        if (cellIndex != lastPaintedCell)
        {
            editCell(cellIndex);
            lastPaintedCell = cellIndex;
        }
    }
    else if (
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
    )
    {
        editCell(cellIndex);
    }
}


bool drawButton(
    Rectangle bounds,
    const char* text,
    bool selected = false
)
{
    Vector2 mouse = GetMousePosition();

    // Convert mouse position into scrollable panel coordinates.
    mouse.y += controlPanelScroll;

    bool hovered = CheckCollisionPointRec(
        mouse,
        bounds
    );

    Color color = BUTTON_COLOR;

    if (selected)
        color = BUTTON_SELECTED_COLOR;
    else if (hovered)
        color = BUTTON_HOVER_COLOR;

    DrawRectangleRec(bounds, color);

    DrawRectangleLinesEx(
        bounds,
        1.5f,
        LIGHTGRAY
    );

    int textWidth = MeasureText(text, 18);

    DrawText(
        text,
        bounds.x + (bounds.width - textWidth) / 2,
        bounds.y + (bounds.height - 18) / 2,
        18,
        TEXT_COLOR
    );

    return hovered &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}


void resetSearch()
{
    // Stop the current visualization.
    comparisonManager.reset();

    visualizers.clear();

    comparisonModeActive = false;

    lastPaintedCell = -1;

    leftCameraDragging = false;
    rightCameraDragging = false;

    leftCameraPanning = false;
    rightCameraPanning = false;

    visualizationAccumulator = 0.0f;

    configureSingleCamera();
}


void drawDimensionSelection()
{
    DrawText(
        "GRID DIMENSION",
        25,
        25,
        22,
        TEXT_COLOR
    );

    Rectangle twoDButton = {
        25, 60, 105, 40
    };

    Rectangle threeDButton = {
        145, 60, 105, 40
    };

    // 2D
    
    if (
        drawButton(
            twoDButton,
            "2D",
            currentDimension == Dimension::TWO_D
        )
    )
    {
        if (currentDimension != Dimension::TWO_D)
        {
            currentDimension = Dimension::TWO_D;

            grid = Grid(GridConfig(
                Dimension::TWO_D,
                DEFAULT_GRID_WIDTH,
                DEFAULT_GRID_HEIGHT,
                1
            ));

            startIndex = -1;
            endIndex = -1;

            resetSearch();
            resetCamera();
        }
    }
    
    // 3D
    
    if (
        drawButton(
            threeDButton,
            "3D",
            currentDimension == Dimension::THREE_D
        )
    )
    {
        if (currentDimension != Dimension::THREE_D)
        {
            currentDimension = Dimension::THREE_D;

            grid = Grid(GridConfig(
                Dimension::THREE_D,
                DEFAULT_GRID_WIDTH,
                DEFAULT_GRID_HEIGHT,
                DEFAULT_GRID_DEPTH
            ));

            startIndex = -1;
            endIndex = -1;

            resetSearch();
            resetCamera();
        }
    }
}


// Algorithm selection menu
void drawAlgorithmSelection()
{
    DrawText(
        "SELECT ALGORITHMS\n(MAX. 2)",
        25,
        120,
        19,
        TEXT_COLOR
    );

    const std::vector<AlgorithmType> algorithms = {
        AlgorithmType::DFS,
        AlgorithmType::BFS,
        AlgorithmType::DIJKSTRA,
        AlgorithmType::GREEDY_BEST_FIRST,
        AlgorithmType::ASTAR
    };

    const std::vector<std::string> names = {
        "Depth-First Search",
        "Breadth-First Search",
        "Dijkstra",
        "Greedy Best-First",
        "A* Search"
    };

    const auto& selected =
        comparisonManager.getSelectedAlgorithms();

    for (size_t i = 0; i < algorithms.size(); i++)
    {
        Rectangle button = {
            25.0f,
            175.0f + i * 45.0f,
            230.0f,
            35.0f
        };

        bool isSelected = std::find(
            selected.begin(),
            selected.end(),
            algorithms[i]
        ) != selected.end();

        if (
            drawButton(
                button,
                names[i].c_str(),
                isSelected
            )
        )
        {
            if (!comparisonManager.isRunning())
            {
                comparisonManager.toggleAlgorithm(
                    algorithms[i]
                );
            }
        }
    }

    DrawText(
        TextFormat(
            "Selected: %i / 2",
            static_cast<int>(selected.size())
        ),
        25,
        405,
        18,
        TEXT_COLOR
    );
}


// Edit mode buttons
void drawEditingControls()
{
    DrawText(
        "GRID EDITING",
        25,
        450,
        22,
        TEXT_COLOR
    );

    Rectangle startButton = {
        25, 490, 230, 35
    };

    Rectangle endButton = {
        25, 535, 230, 35
    };

    Rectangle barrierButton = {
        25, 580, 230, 35
    };

    Rectangle resetCameraButton = {
        25, 625, 230, 35
    };

    if (drawButton(
        startButton,
        "Choose start",
        editMode == EditMode::START
    ))
    {
        editMode = EditMode::START;
    }

    if (drawButton(
        endButton,
        "Choose end",
        editMode == EditMode::END
    ))
    {
        editMode = EditMode::END;
    }

    if (drawButton(
        barrierButton,
        "Draw barriers",
        editMode == EditMode::BARRIER
    ))
    {
        editMode = EditMode::BARRIER;
    }

    if (drawButton(
        resetCameraButton,
        "Reset Camera"
    ))
    {
        resetCamera();
    }

    DrawText(
        "CTRL + Left mouse: Edit grid",
        25, 675, 14, LIGHTGRAY
    );

    DrawText(
        "Left mouse: Rotate camera",
        25, 698, 14, LIGHTGRAY
    );

    DrawText(
        "Right mouse: Pan camera",
        25, 721, 14, LIGHTGRAY
    );

    DrawText(
        "Mouse wheel: Zoom",
        25, 744, 14, LIGHTGRAY
    );
}


// Playback controls
void drawSpeedSlider()
{
    DrawText(
        "VISUALIZATION SPEED",
        25,
        785,
        18,
        TEXT_COLOR
    );

    Rectangle sliderBounds = {
        25, 820, 230, 8
    };

    Vector2 mouse = GetMousePosition();
    mouse.y += controlPanelScroll;

    // Update slider while dragging.
    if (
        IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(
            mouse,
            {
                sliderBounds.x - 5,
                sliderBounds.y - 12,
                sliderBounds.width + 10,
                sliderBounds.height + 24
            }
        )
    )
    {
        float percentage =
            (mouse.x - sliderBounds.x) /
            sliderBounds.width;

        percentage = Clamp(
            percentage,
            0.0f,
            1.0f
        );

        visualizationSpeed =
            MIN_VISUALIZATION_SPEED +
            percentage *
            (
                MAX_VISUALIZATION_SPEED -
                MIN_VISUALIZATION_SPEED
            );
    }

    float percentage =
        (visualizationSpeed - MIN_VISUALIZATION_SPEED) /
        (
            MAX_VISUALIZATION_SPEED -
            MIN_VISUALIZATION_SPEED
        );

    DrawRectangleRec(
        sliderBounds,
        DARKGRAY
    );

    DrawRectangle(
        sliderBounds.x,
        sliderBounds.y,
        sliderBounds.width * percentage,
        sliderBounds.height,
        SKYBLUE
    );

    DrawCircle(
        sliderBounds.x +
            sliderBounds.width * percentage,
        sliderBounds.y + sliderBounds.height / 2,
        9,
        RAYWHITE
    );

    DrawText(
        TextFormat(
            "%.1f updates/sec",
            visualizationSpeed
        ),
        25,
        840,
        16,
        LIGHTGRAY
    );
}


void drawPlaybackControls()
{
    Rectangle playButton = {
        25, 880, 100, 40
    };

    Rectangle pauseButton = {
        135, 880, 100, 40
    };

    Rectangle resetButton = {
        25, 930, 210, 40
    };

    // Play / Resume
    if (drawButton(playButton, "Play"))
    {
        if (comparisonManager.isPaused())
        {
            comparisonManager.resume();
        }
        else if (!comparisonManager.isRunning())
        {
            const auto& selected =
                comparisonManager.getSelectedAlgorithms();

            if (
                !selected.empty() &&
                startIndex >= 0 &&
                endIndex >= 0
            )
            {
                if (selected.size() == 2)
                {
                    configureComparisonCameras();
                }
                else
                {
                    configureSingleCamera();
                }

                visualizationAccumulator = 0.0f;

                comparisonManager.start(
                    grid,
                    startIndex,
                    endIndex
                );

                visualizers.clear();

                for (const auto& view :
                    comparisonManager.getViews())
                {
                    visualizers.emplace_back(view);
                }

                comparisonModeActive =
                    selected.size() == 2;
            }
        }
    }

    // Pause
    if (drawButton(pauseButton, "Pause"))
    {
        comparisonManager.pause();
    }

    // Reset
    if (drawButton(resetButton, "Reset"))
    {
        resetSearch();
    }
}


void updateVisualizers()
{
    visualizationAccumulator += GetFrameTime();

    const float updateInterval =
        1.0f / visualizationSpeed;

    while (
        visualizationAccumulator >= updateInterval
    )
    {
        comparisonManager.update();

        const auto& views =
            comparisonManager.getViews();

        if (visualizers.size() == views.size())
        {
            for (size_t i = 0; i < views.size(); i++)
            {
                visualizers[i].update(views[i]);
            }
        }

        visualizationAccumulator -= updateInterval;

        if (!comparisonManager.isRunning())
            break;
    }
}


// Draw one visualization viewport
void drawViewport(
    int viewportX,
    int viewportWidth,
    Camera3D& camera,
    const AlgorithmVisualizer& visualizer
)
{
    if (viewportWidth <= 0)
        return;

    beginViewport3D(
        camera,
        viewportX,
        viewportWidth
    );

    drawGrid(visualizer);

    endViewport3D();

    // Algorithm name
    DrawText(
        visualizer.getName().c_str(),
        viewportX + 25,
        25,
        25,
        TEXT_COLOR
    );

    // Individual algorithm status
    if (visualizer.isFinished())
    {
        DrawText(
            "FINISHED",
            viewportX + 25,
            60,
            18,
            GREEN
        );
    }
    else if (comparisonManager.isPaused())
    {
        DrawText(
            "PAUSED",
            viewportX + 25,
            60,
            18,
            YELLOW
        );
    }
    else
    {
        DrawText(
            "RUNNING",
            viewportX + 25,
            60,
            18,
            YELLOW
        );
    }
}


void drawEditableViewport(
    Camera3D& camera,
    int viewportX,
    int viewportWidth
)
{
    if (viewportWidth <= 0)
        return;

    beginViewport3D(
        camera,
        viewportX,
        viewportWidth
    );

    drawEditableGrid();

    endViewport3D();
}

// Draw algorithm statistics
void drawStatistics(
    const AlgorithmStatistics& stats,
    int x,
    int y,
    int width
)
{
    const int panelHeight = 155;

    // Background
    DrawRectangle(
        x,
        y,
        width,
        panelHeight,
        { 32, 38, 51, 245 }
    );

    DrawRectangleLines(
        x,
        y,
        width,
        panelHeight,
        GRAY
    );

    // Title
    DrawText(
        "ALGORITHM STATISTICS",
        x + 15,
        y + 12,
        20,
        RAYWHITE
    );

    // Visited nodes
    DrawText(
        TextFormat(
            "Visited nodes: %i",
            stats.visitedNodes
        ),
        x + 15,
        y + 45,
        17,
        LIGHTGRAY
    );

    // Path length
    DrawText(
        TextFormat(
            "Path length: %i",
            stats.pathLength
        ),
        x + 15,
        y + 70,
        17,
        LIGHTGRAY
    );

    // Path found
    DrawText(
        stats.pathFound
            ? "Path found: YES"
            : "Path found: NO",
        x + 15,
        y + 95,
        17,
        stats.pathFound ? GREEN : RED
    );

    // Explored percentage
    DrawText(
        TextFormat(
            "Explored: %.2f%%",
            stats.exploredPercentage
        ),
        x + 15,
        y + 120,
        17,
        LIGHTGRAY
    );
}


void drawVisualizationViewports()
{
    updateViewportConfiguration();

    DrawRectangle(
        leftViewportX,
        0,
        leftViewportWidth,
        SCREEN_HEIGHT,
        BACKGROUND_COLOR
    );

    if (comparisonModeActive && visualizers.size() == 2)
    {
        DrawRectangle(
            rightViewportX,
            0,
            rightViewportWidth,
            SCREEN_HEIGHT,
            BACKGROUND_COLOR
        );

        DrawLine(
            rightViewportX,
            0,
            rightViewportX,
            SCREEN_HEIGHT,
            GRAY
        );
    }

    // No algorithm selected
    if (visualizers.empty())
    {
        drawEditableViewport(
            leftCamera,
            leftViewportX,
            leftViewportWidth
        );

        DrawText(
            "Configure your grid and select algorithms",
            leftViewportX + 30,
            20,
            18,
            LIGHTGRAY
        );

        DrawText(
            "Press PLAY to start the visualization",
            leftViewportX + 30,
            50,
            18,
            LIGHTGRAY
        );

        return;
    }

    // One algorithm
    if (visualizers.size() == 1)
    {
        drawViewport(
            leftViewportX,
            leftViewportWidth,
            leftCamera,
            visualizers[0]
        );

        // Display statistics after completion.
        if (
            !comparisonManager.isRunning() &&
            comparisonManager.allFinished() &&
            comparisonManager.hasResults()
        )
        {
            const auto& views =
                comparisonManager.getViews();

            if (!views.empty())
            {
                drawStatistics(
                    views[0].statistics,
                    leftViewportX + 20,
                    SCREEN_HEIGHT - 175,
                    leftViewportWidth - 40
                );
            }
        }

        return;
    }

    // Two algorithms
    if (visualizers.size() == 2)
    {
        drawViewport(
            leftViewportX,
            leftViewportWidth,
            leftCamera,
            visualizers[0]
        );

        drawViewport(
            rightViewportX,
            rightViewportWidth,
            rightCamera,
            visualizers[1]
        );

        // Display statistics after both algorithms finish.
        if (
            !comparisonManager.isRunning() &&
            comparisonManager.allFinished() &&
            comparisonManager.hasResults()
        )
        {
            const auto& views =
                comparisonManager.getViews();

            if (views.size() == 2)
            {
                // First algorithm
                drawStatistics(
                    views[0].statistics,
                    leftViewportX + 15,
                    SCREEN_HEIGHT - 175,
                    leftViewportWidth - 30
                );

                // Second algorithm
                drawStatistics(
                    views[1].statistics,
                    rightViewportX + 15,
                    SCREEN_HEIGHT - 175,
                    rightViewportWidth - 30
                );
            }
        }
    }
}


void updateControlPanelScroll()
{
    Vector2 mouse = GetMousePosition();

    // Only scroll when the mouse is over the control panel.
    if (mouse.x >= 0 &&
        mouse.x < CONTROL_PANEL_WIDTH)
    {
        float wheel = GetMouseWheelMove();

        controlPanelScroll -= wheel * 40.0f;
    }

    // Calculate maximum scrolling distance.
    float maxScroll = std::max(
        0.0f,
        CONTROL_PANEL_CONTENT_HEIGHT -
        static_cast<float>(GetScreenHeight())
    );

    // Keep scrolling within valid limits.
    controlPanelScroll = Clamp(
        controlPanelScroll,
        0.0f,
        maxScroll
    );
}


void updateApplicationInput()
{
    if (WindowShouldClose())
        return;

    updateControlPanelScroll();

    // Update camera controls.
    updateCamera(
        leftCamera,
        leftCameraDragging,
        leftCameraPanning,
        leftViewportX,
        leftViewportWidth
    );

    if (
        comparisonModeActive &&
        visualizers.size() == 2
    )
    {
        updateCamera(
            rightCamera,
            rightCameraDragging,
            rightCameraPanning,
            rightViewportX,
            rightViewportWidth
        );
    }

    // Edit the grid only before visualization starts.
    if (visualizers.empty())
    {
        updateGridInput(
            leftCamera,
            leftViewportX,
            leftViewportWidth
        );
    }

    // Update algorithm animations.
    if (
        comparisonManager.isRunning() &&
        !comparisonManager.isPaused()
    )
    {
        updateVisualizers();
    }
}


void initializeApplication()
{
    // Enable anti-aliasing and window resizing.
    SetConfigFlags(
        FLAG_MSAA_4X_HINT |
        FLAG_WINDOW_RESIZABLE
    );

    // Create a normal desktop window.
    InitWindow(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        "PathAlgoStudio - 2D / 3D Pathfinding Algorithms Visualizer"
    );

    // Centre window on the monitor.
    int monitor = GetCurrentMonitor();

    int monitorWidth = GetMonitorWidth(monitor);
    int monitorHeight = GetMonitorHeight(monitor);

    SetWindowPosition(
        (monitorWidth - SCREEN_WIDTH) / 2,
        (monitorHeight - SCREEN_HEIGHT) / 2
    );

    SetTargetFPS(60);

    // Initialize grid.
    grid = Grid(
        GridConfig(
            currentDimension,
            DEFAULT_GRID_WIDTH,
            DEFAULT_GRID_HEIGHT,
            1
        )
    );

    // Initialize camera.
    configureSingleCamera();

    // Initialize viewport dimensions.
    updateViewportConfiguration();

    // Initialize editing state.
    startIndex = -1;
    endIndex = -1;

    lastPaintedCell = -1;

    editMode = EditMode::NONE;

    comparisonModeActive = false;

    visualizers.clear();
}


void drawControlPanel()
{
    // Draw fixed panel background.
    DrawRectangle(
        0,
        0,
        CONTROL_PANEL_WIDTH,
        SCREEN_HEIGHT,
        {
            32, 38, 51, 255
        }
    );

    DrawLine(
        CONTROL_PANEL_WIDTH,
        0,
        CONTROL_PANEL_WIDTH,
        SCREEN_HEIGHT,
        GRAY
    );

    // Clip everything to the control panel.
    BeginScissorMode(
        0,
        0,
        CONTROL_PANEL_WIDTH,
        SCREEN_HEIGHT
    );

    // Move the entire control panel content upwards.
    rlPushMatrix();

    rlTranslatef(
        0.0f,
        -controlPanelScroll,
        0.0f
    );

    // Draw all existing controls.
    drawDimensionSelection();

    drawAlgorithmSelection();

    drawEditingControls();

    drawSpeedSlider();

    drawPlaybackControls();

    rlPopMatrix();

    EndScissorMode();

    // ------------------------------------------------
    // Draw scrollbar (fixed position)
    // ------------------------------------------------

    float contentHeight =
        CONTROL_PANEL_CONTENT_HEIGHT;

    float visibleHeight =
        static_cast<float>(SCREEN_HEIGHT);

    if (contentHeight > visibleHeight)
    {
        float scrollbarHeight =
            visibleHeight *
            (visibleHeight / contentHeight);

        scrollbarHeight = std::max(
            35.0f,
            scrollbarHeight
        );

        float maxScroll =
            contentHeight - visibleHeight;

        float scrollPercentage =
            maxScroll > 0.0f
                ? controlPanelScroll / maxScroll
                : 0.0f;

        float scrollbarY =
            scrollPercentage *
            (visibleHeight - scrollbarHeight);

        const int scrollbarX =
            CONTROL_PANEL_WIDTH - SCROLLBAR_WIDTH - 3;

        // Scrollbar background.
        DrawRectangle(
            scrollbarX,
            0,
            SCROLLBAR_WIDTH,
            SCREEN_HEIGHT,
            DARKGRAY
        );

        // Scrollbar handle.
        DrawRectangleRounded(
            {
                static_cast<float>(scrollbarX),
                scrollbarY,
                static_cast<float>(SCROLLBAR_WIDTH),
                scrollbarHeight
            },
            0.8f,
            8,
            LIGHTGRAY
        );
    }
}


void drawApplication()
{
    BeginDrawing();

    ClearBackground(BACKGROUND_COLOR);

    // Draw the 2D/3D grid and algorithm visualizations.
    drawVisualizationViewports();

    // Draw controls on top of the viewport.
    drawControlPanel();

    EndDrawing();
}


void shutdownApplication()
{
    visualizers.clear();

    CloseWindow();
}


int main()
{
    initializeApplication();

    while (!WindowShouldClose())
    {
        SCREEN_WIDTH = GetScreenWidth();
        SCREEN_HEIGHT = GetScreenHeight();

        updateViewportConfiguration();

        updateApplicationInput();

        drawApplication();
    }

    shutdownApplication();

    return 0;
}