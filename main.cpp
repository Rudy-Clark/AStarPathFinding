#include <iostream>
#include <float.h>
#include <math.h>
#include <raylib.h>
#include <string>
#include <vector>

#include "AStar.hpp"
#include "Commands.hpp"

#define FLT_STRING_SIZE (1+1+1+(FLT_DECIMAL_DIG-1)+1+1+ 4   +1)
#define SIZE 256

// TODO 
// I have one problem I wanna
// 

namespace Settings {
    static constexpr int screenWidth{ 720 };
    static constexpr int screenHeight{ 720 };
}

void generateRandomObstacles(std::vector<Vector2>& obstacles, int numCells, int probabilityPercentage = 50) {
    for (int i{}; i < (numCells * numCells); i++) {
        float x = i % numCells;
        float y = std::floor(i / numCells);
        int r = GetRandomValue(0, 100);
        // Probability appear collision cell 30%
        if (r <= probabilityPercentage) {
            obstacles.push_back({x, y});
        }
    }
}

void genObstacles(Vector2* obstacles, int numCells) {
    for (int i = 0; i < SIZE; i++) {
        obstacles[i] = { (float)GetRandomValue(0, numCells - 1), (float)GetRandomValue(0, numCells - 1) };
    }
}

int main() {
    InitWindow(Settings::screenWidth, Settings::screenHeight, "Window");
    SetTargetFPS(GetMonitorRefreshRate(0));
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    Shader gridShader = LoadShader("./Shaders/grid.vert", "./Shaders/grid.frag");
    //Shader gridShader = LoadShaderFromMemory(gridShaderVertexCode, gridShaderFragmentCode);
    // Set up uniform locations
    int screenSizeLoc = GetShaderLocation(gridShader, "screenSize");
    int gridSpacingLoc = GetShaderLocation(gridShader, "gridSpacing");
    int gridColorLoc = GetShaderLocation(gridShader, "gridColor");
    int activeCellLoc = GetShaderLocation(gridShader, "u_activeCell");
    int activeCellColorLoc = GetShaderLocation(gridShader, "u_activeCellColor");
    
    // obstacles
    int obstacleListLoc = GetShaderLocation(gridShader, "ua_obstacles");
    // obstacle color
    int obstacleColorLoc = GetShaderLocation(gridShader, "u_obstacleColor");

    // Set screen size uniform
    Vector2 screenSize = { (float)GetScreenWidth(), (float)GetScreenHeight() };
    SetShaderValue(gridShader, screenSizeLoc, &screenSize, SHADER_UNIFORM_VEC2);

    // Set grid color (white) and spacing
    Vector3 gridColor = { 0.0f, 0.0f, 0.0f };
    float gridSpacing = 24.0f;
    SetShaderValue(gridShader, gridSpacingLoc, &gridSpacing, SHADER_UNIFORM_FLOAT);
    SetShaderValue(gridShader, gridColorLoc, &gridColor, SHADER_UNIFORM_VEC3);
    // obstacle color;
    Vector3 ObstacleColor = {0.51f, 0.51f, 0.51f};
    SetShaderValue(gridShader, obstacleColorLoc, &ObstacleColor, SHADER_UNIFORM_VEC3);

    const int numCells = (GetScreenWidth() / gridSpacing);
    
    Vector2 obstaclesRow[SIZE];
    size_t s = std::size(obstaclesRow);

    // generate obstacles
    genObstacles(obstaclesRow, numCells);
    AStar::Generator generator;
    generator.setDiagonalMovement(false);
    generator.setWorldSize({ numCells, numCells });
    // define collision cells
    for (int i{ 0 }; i < SIZE; ++i) {
        generator.addCollision({ (int)obstaclesRow[i].x, (int)obstaclesRow[i].y });
    }

    Commands comm;
    comm.Push({ 1.f, 1.f });

    int obstaclesRowLoc = GetShaderLocation(gridShader, "u_obstacles");
    SetShaderValueV(gridShader, obstaclesRowLoc, obstaclesRow, SHADER_UNIFORM_VEC2, SIZE);

    Vector2 mousePos = { 0.0, 0.0 };
    SetShaderValue(gridShader, activeCellLoc, &mousePos, SHADER_UNIFORM_VEC2);
    Vector3 activeCellColor = { 0.f, 228.f / 255.f, 48.f / 255.f };
    SetShaderValue(gridShader, activeCellColorLoc, &activeCellColor, SHADER_UNIFORM_VEC3);

    std::string gridSizeText = "Grid Space: " + std::to_string((int)gridSpacing);
    std::string posText = "Vec2(" + std::to_string((int)mousePos.x) + ", " + std::to_string((int)mousePos.y) + ")";
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // set grid cell active by click mouse left button
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            mousePos = { std::floor(GetMousePosition().x / gridSpacing), std::floor((GetScreenHeight() - GetMousePosition().y) / gridSpacing) };
            SetShaderValue(gridShader, activeCellLoc, &mousePos, SHADER_UNIFORM_VEC2);
            posText = "Vec2(" + std::to_string((int)mousePos.x) + ", " + std::to_string((int)mousePos.y) + ")";
            comm.Push(mousePos);
        }

        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            comm.ClearCommands();
        }

        if (IsKeyPressed(KEY_F)) {
            if (comm.GetSize() >= 2) {
                for (int i{ 0 }; i < SIZE; ++i) {
                    generator.addCollision({ (int)obstaclesRow[i].x, (int)obstaclesRow[i].y });
                }
                generator.findPath({ (int)comm.GetStart().x,  (int)comm.GetStart().y }, { (int)comm.GetEnd().x,  (int)comm.GetEnd().y });
            }
        }

        /*if (IsKeyPressed(KEY_PAGE_UP)) {
            ++gridSpacing;
            SetShaderValue(gridShader, gridSpacingLoc, &gridSpacing, SHADER_UNIFORM_FLOAT);
            gridSizeText = "Grid Space: " + std::to_string((int)gridSpacing);
        }

        if (IsKeyPressed(KEY_PAGE_DOWN)) {
            --gridSpacing;
            SetShaderValue(gridShader, gridSpacingLoc, &gridSpacing, SHADER_UNIFORM_FLOAT);
            gridSizeText = "Grid Space: " + std::to_string((int)gridSpacing);
        }*/

        if (IsKeyPressed(KEY_R)) {
            generator.clearCollisions();
            genObstacles(obstaclesRow, numCells);
            SetShaderValueV(gridShader, obstaclesRowLoc, obstaclesRow, SHADER_UNIFORM_VEC2, SIZE);
        }

        BeginShaderMode(gridShader);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), WHITE);
        EndShaderMode();

        DrawText(gridSizeText.c_str(), GetScreenWidth() - 200, 8, 28, RED);
        DrawText(posText.c_str(), GetScreenWidth() - 150, 48, 16, RED);
        DrawFPS(14, 8);
        EndDrawing();
    }

    UnloadShader(gridShader);
    CloseWindow();

    return 0;
}
