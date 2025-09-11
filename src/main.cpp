#include <raylib.h>
#include <mlpack/core.hpp>
#include <armadillo>
#include <dansweeperml/render.h>

#include <dansweeperml/controller.h>

int main() {

    const int screenWidth = 800;
    const int screenHeight = 600;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_VSYNC_HINT);
    SetTargetFPS(240);
    Grid::Grid* currentGrid = new Grid::Grid(17, 17, 0);

    InitWindow(screenWidth, screenHeight, "dansweeperml");

    Camera2D camera;
    Render::initializeRender(camera, currentGrid);
    Controller::initializeController(camera, currentGrid);

    Render::loadTexture();
    Font customFont = LoadFontEx("../resources/ProggyClean.ttf", 13, 0, 250);
    SetTextureFilter(customFont.texture, TEXTURE_FILTER_POINT);

    // Main loop
    while (!WindowShouldClose()) {

        Controller::cameraZoom();
        Controller::cameraPan();
        Controller::cameraHover();

        BeginDrawing();

        ClearBackground(BLACK);
        Render::renderThread();
        DrawTextEx(customFont, std::to_string(GetFPS()).c_str(), {10, 10}, 13, 1, WHITE);
        DrawTextEx(customFont, std::format("coords: {} {}", Controller::getCoordinates().first, Controller::getCoordinates().second).c_str(), {10, 25}, 13, 1, WHITE);

        EndDrawing();

    }

    Render::unloadTexture();
    CloseWindow();
    return 0;
}
