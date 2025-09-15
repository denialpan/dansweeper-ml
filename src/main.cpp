#include <raylib.h>
#include <mlpack/core.hpp>
#include <armadillo>

#include <dansweeperml/core/render.h>
#include <dansweeperml/core/controller.h>
#include <thread>
#include <memory>

#include "dansweeperml/solver/algorithm/linearscan.h"

enum RunType {
    RUN_ALGORITHM,
    RUN_AGENT,
    LEARN_AGENT,
};

static RunType runtype = RUN_ALGORITHM;
static int iterateRuntype;

void debug(Font font, Grid::Grid* grid) {

    std::vector<std::string> listOfText;
    Grid::GridMetadata metadata = grid->getMetadata();
    std::vector<std::vector<Grid::Cell>> cells = grid->getCells();
    auto [cx, cy] = Controller::getCoordinates();

    std::string runtypeString = "";

    listOfText.push_back(std::format("created by daniel pan"));
    listOfText.push_back(std::format("fps: {}", GetFPS()));
    listOfText.push_back(std::format("dims: {}x{}", metadata.width, metadata.height));
    listOfText.push_back(std::format("mines: {}", metadata.mineNum));
    listOfText.push_back(std::format("prng: {}", metadata.prng));
    listOfText.push_back(std::format("safe: {}, {}", metadata.safeX, metadata.safeY));
    listOfText.push_back(std::format("time: {}", metadata.time));

    switch (runtype) {
        case RUN_ALGORITHM:
            runtypeString = "algorithm";
            break;
        case RUN_AGENT:
            runtypeString = "agent";
            break;
        case LEARN_AGENT:
            runtypeString = "learn agent";
            break;
        default:
            std::cout << "how you get here" << std::endl;
            break;
    }

    listOfText.push_back(std::format("run type: {}", runtypeString));

    if (cx >= 0 && cy >= 0 && cx < metadata.width && cy < metadata.height) {
        listOfText.push_back(std::format("coords: {}, {}", cx, cy));
        listOfText.push_back(std::format("mine: {}", cells[cy][cx].content == Grid::CELL_MINE));
        listOfText.push_back(std::format("adjc: {}", cells[cy][cx].adjacentMines));
    }

    for (int i = 0; i < listOfText.size(); i++) {
        DrawTextEx(font, listOfText[i].c_str(), {10, 15.0f * i + 10}, 13, 1, WHITE);
    }

}

std::jthread solverThread(Grid::Grid* grid) {

    using namespace std::chrono_literals;

    return std::jthread([grid](std::stop_token st) {

        std::unique_ptr<ISolver> solver = std::make_unique<algorithmlinearscan::LinearScan>();

        while (!st.stop_requested()) {
            solver->step(*grid);
            std::this_thread::sleep_for(1ms);
        }

    });

}


int main() {

    const int screenWidth = 800;
    const int screenHeight = 600;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_VSYNC_HINT);
    SetTargetFPS(240);
    Grid::Grid* currentGrid = new Grid::Grid(17, 17, 0.25f);

    InitWindow(screenWidth, screenHeight, "dansweeperml");

    Camera2D camera;
    Render::initializeRender(camera, currentGrid);
    Controller::initializeController(camera, currentGrid);

    Render::loadTexture();
    Font customFont = LoadFontEx("../resources/ProggyClean.ttf", 13, 0, 250);
    SetTextureFilter(customFont.texture, TEXTURE_FILTER_POINT);

    currentGrid->generateGrid(4, 4);

    std::jthread walker = solverThread(currentGrid);

    while (!WindowShouldClose()) {

        Controller::cameraZoom();
        Controller::cameraPan();
        Controller::cameraHover();

        switch (runtype) {
            case RUN_ALGORITHM:
                break;
            case RUN_AGENT:

                break;
            case LEARN_AGENT:

                break;
            default:
                std::cout << "how you get here" << std::endl;
                break;
        }

        // debug new board
        if (IsKeyDown(KEY_SPACE)) {
            currentGrid->generateGrid(4, 4);
        }

        if (IsKeyPressed(KEY_LEFT)) {
            iterateRuntype--;
            runtype = static_cast<RunType>((static_cast<RunType>(iterateRuntype + 3)) % 3);
        }

        if (IsKeyPressed(KEY_RIGHT)) {
            iterateRuntype++;
            runtype = static_cast<RunType>((static_cast<RunType>(iterateRuntype + 3)) % 3);
        }

        BeginDrawing();

        ClearBackground(BLACK);
        Render::renderThread();
        currentGrid->updateTimer();

        debug(customFont, currentGrid);
        EndDrawing();

    }

    Render::unloadTexture();
    CloseWindow();
    return 0;
}
