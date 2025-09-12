#include <raylib.h>
#include <mlpack/core.hpp>
#include <armadillo>
#include <dansweeperml/core/render.h>

#include <../include/dansweeperml/core/controller.h>

void debug(Font font) {

    std::vector<std::string> listOfText;

    listOfText.push_back(std::format("created by daniel pan"));
    listOfText.push_back(std::format("fps: {}", GetFPS()));
    listOfText.push_back(std::format("coords: {}, {}", Controller::getCoordinates().first, Controller::getCoordinates().second));

    for (int i = 0; i < listOfText.size(); i++) {
        DrawTextEx(font, listOfText[i].c_str(), {10, 15.0f * i + 10}, 13, 1, WHITE);
    }

}

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


    while (!WindowShouldClose()) {

        Controller::cameraZoom();
        Controller::cameraPan();
        Controller::cameraHover();

        BeginDrawing();

        ClearBackground(BLACK);
        Render::renderThread();

        debug(customFont);
        EndDrawing();

    }

    Render::unloadTexture();
    CloseWindow();
    return 0;
}
