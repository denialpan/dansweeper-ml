#include <raylib.h>
#include <mlpack/core.hpp>
#include <armadillo>
#include <dansweeperml/core/render.h>

#include <../include/dansweeperml/core/controller.h>

void debug(Font font, Grid::Grid* grid) {

    std::vector<std::string> listOfText;
    Grid::GridMetadata metadata = grid->getMetadata();
    std::vector<std::vector<Grid::Cell>> cells = grid->getCells();
    auto [cx, cy] = Controller::getCoordinates();

    listOfText.push_back(std::format("created by daniel pan"));
    listOfText.push_back(std::format("fps: {}", GetFPS()));
    listOfText.push_back(std::format("dims: {}x{}", metadata.width, metadata.height));
    listOfText.push_back(std::format("mines: {}", metadata.mineNum));
    listOfText.push_back(std::format("prng: {}", metadata.prng));
    listOfText.push_back(std::format("safe: {}, {}", metadata.safeX, metadata.safeY));

    if (cx >= 0 && cy >= 0 && cx < metadata.width && cy < metadata.height) {
        listOfText.push_back(std::format("coords: {}, {}", cx, cy));
        listOfText.push_back(std::format("mine: {}", cells[cy][cx].content == Grid::CELL_MINE));
        listOfText.push_back(std::format("adjc: {}", cells[cy][cx].adjacentMines));
    }


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
    Grid::Grid* currentGrid = new Grid::Grid(9, 9, 0.25f);

    InitWindow(screenWidth, screenHeight, "dansweeperml");

    Camera2D camera;
    Render::initializeRender(camera, currentGrid);
    Controller::initializeController(camera, currentGrid);

    Render::loadTexture();
    Font customFont = LoadFontEx("../resources/ProggyClean.ttf", 13, 0, 250);
    SetTextureFilter(customFont.texture, TEXTURE_FILTER_POINT);

    currentGrid->generateGrid(4, 4);

    while (!WindowShouldClose()) {

        Controller::cameraZoom();
        Controller::cameraPan();
        Controller::cameraHover();

        BeginDrawing();

        ClearBackground(BLACK);
        Render::renderThread();

        debug(customFont, currentGrid);
        EndDrawing();

    }

    Render::unloadTexture();
    CloseWindow();
    return 0;
}
