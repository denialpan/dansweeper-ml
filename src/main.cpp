#include <raylib.h>
#include <mlpack/core.hpp>
#include <armadillo>
#include <dansweeperml/render.h>

int main() {

    const int screenWidth = 800;
    const int screenHeight = 600;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    Grid::Grid* currentGrid = new Grid::Grid(1000, 1000, 0);

    InitWindow(screenWidth, screenHeight, "dansweeperml");

    Render::loadTexture();
    Render::initializeCamera(currentGrid);

    // Main loop
    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(BLACK);
        Render::renderThread(currentGrid);
        DrawText(std::to_string(GetFPS()).c_str(), 20, 20, 20, DARKGRAY);

        EndDrawing();
    }

    Render::unloadTexture();
    CloseWindow();
    return 0;
}
