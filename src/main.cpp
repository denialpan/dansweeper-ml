#include <raylib.h>
#include <mlpack/core.hpp>
#include <armadillo>

#include <dansweeperml/core/render.h>
#include <dansweeperml/core/controller.h>
#include <thread>
#include <memory>

#include <dansweeperml/solver/isolver.h>

#include "dansweeperml/solver/algorithm/bfsoptimized.h"
#include "dansweeperml/solver/algorithm/linearscan.h"


enum RunType {
    RUN_ALGORITHM,
    RUN_AGENT,
    LEARN_AGENT,
};

struct SolverStats {
    int steps = 0;
    int totalSteps = 0;
    float time = 0.0f;
    float totalTime = 0.0f;
    int boardsRun = 1;
    int win = 0;
    int lose = 0;
    float winrate = 0.0f;
    float averageSteps = 0.0f;
    float averageTime = 0.0f;
};

static RunType runtype = RUN_ALGORITHM;
static int iterateRuntype;

std::vector<std::unique_ptr<ISolver>> solvers;
static int algorithmSelectionIndex = 1;

std::atomic<bool> stepRequested = false;
static SolverStats stats;

void solverstats(const Font &font) {
    std::vector<std::string> listOfText;

    listOfText.push_back(std::format("average steps: {}", stats.averageSteps));
    listOfText.push_back(std::format("average time: {}", stats.averageTime));
    listOfText.push_back(std::format("winrate: {}", stats.winrate));
    listOfText.push_back(std::format("boards run: {}", stats.boardsRun));
    listOfText.push_back(std::format("time: {}", stats.time));
    listOfText.push_back(std::format("steps: {}", stats.steps));
    listOfText.push_back(std::format("lose: {}", stats.lose));
    listOfText.push_back(std::format("win: {}", stats.win));

    for (int i = 0; i < listOfText.size(); i++) {
        DrawTextEx(font, listOfText[i].c_str(), {10, GetScreenHeight() / 2 - (15.0f * i + 20)}, 13, 1, WHITE);
    }
}

void controls(const Font &font) {

    std::vector<std::string> listOfText;

    listOfText.push_back(std::format("[->]: step forward once"));
    listOfText.push_back(std::format("[a] [d]: cycle main solver type"));
    listOfText.push_back(std::format("[w] [s]: cycle algorithm type"));
    listOfText.push_back(std::format("[space]: reset board"));

    for (int i = 0; i < listOfText.size(); i++) {
        DrawTextEx(font, listOfText[i].c_str(), {10, GetScreenHeight() - (15.0f * i + 20)}, 13, 1, WHITE);
    }
}

void debug(const Font &font, Grid::Grid* grid) {

    std::vector<std::string> listOfText;
    Grid::GridMetadata metadata = grid->getMetadata();
    std::vector<std::vector<Grid::Cell>> cells = grid->getCells();
    auto [cx, cy] = Controller::getCoordinates();

    std::string runtypeString = "";

    listOfText.push_back(std::format("created by daniel pan"));
    listOfText.push_back(std::format("fps: {}", GetFPS()));
    listOfText.push_back(std::format("dims: {}x{}", metadata.width, metadata.height));
    listOfText.push_back(std::format("mines: {}, density: {}", metadata.mineNum, static_cast<float>(metadata.mineNum) / (metadata.width * metadata.height)));
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

std::jthread solverThread(Grid::Grid* grid, int& selectionIndex, bool& autoRunSolver) {

    using namespace std::chrono_literals;

    auto resetSolverStats = [](SolverStats s) {
        s.steps = 0;
        s.boardsRun = 0;
        s.win = 0;
        s.lose = 0;
        s.winrate = 0.0f;
        s.averageSteps = 0.0f;
        s.averageTime = 0.0f;
        s.time = 0;
        s.totalSteps = 0;
        s.totalTime = 0;
    };

    return std::jthread([grid, &selectionIndex, &autoRunSolver, resetSolverStats](std::stop_token st) {

        // register algorithmic solvers
        solvers.push_back(std::make_unique<algorithmlinearscan::LinearScan>());
        solvers.push_back(std::make_unique<algorithmbfsoptimized::BFSUnoptimized>());

        size_t current = solvers.empty() ? 0 : (selectionIndex % solvers.size());
        ISolver* solver = solvers[current].get();

        auto resetRun = [&] {
            const auto meta = grid->getMetadata();

            stats.time = meta.time;
            stats.totalTime += meta.time;
            stats.totalSteps += solver->getSteps();

            std::this_thread::sleep_for(1000ms);

            grid->generateGrid(meta.width / 2, meta.height / 2);
            solver = solvers[current].get();
            solver->reset();
            Render::resetHighlightTiles();

            stats.steps = 0;
            stats.boardsRun++;

            stats.averageSteps = stats.totalSteps / stats.boardsRun;
            stats.averageTime = stats.totalTime / stats.boardsRun;
        };

        while (!st.stop_requested()) {

            size_t now = solvers.empty() ? 0 : (selectionIndex % solvers.size());
            if (now != current) {
                current = now;
                solver = solvers[current].get();
                solver->reset();
                resetSolverStats(stats);
            }

            if (autoRunSolver || stepRequested.exchange(false)) {
                stats.steps++;
                if (!solver->step(*grid)) {
                    resetRun();
                }
            }

            auto currentGridState = grid->getMetadata().gridState;

            if (currentGridState == Grid::FINISHED_LOSE || currentGridState == Grid::FINISHED_WIN) {

                switch (currentGridState) {
                    case Grid::FINISHED_LOSE:
                        stats.lose++;
                        break;
                    case Grid::FINISHED_WIN:
                        stats.win++;
                        break;
                }

                stats.winrate = static_cast<float>(stats.win) / stats.boardsRun;
                resetRun();

            }

            std::this_thread::sleep_for(50ms);

        }

    });

}


int main() {

    const int screenWidth = 800;
    const int screenHeight = 600;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_VSYNC_HINT);
    SetTargetFPS(240);
    Grid::Grid* currentGrid = new Grid::Grid(9, 9, 10);

    InitWindow(screenWidth, screenHeight, "dansweeperml");

    Camera2D camera;
    Render::initializeRender(camera, currentGrid);
    Controller::initializeController(camera, currentGrid);

    Render::loadTexture();
    Font customFont = LoadFontEx("../resources/ProggyClean.ttf", 13, 0, 250);
    SetTextureFilter(customFont.texture, TEXTURE_FILTER_POINT);

    bool autoRunSolver = true;

    currentGrid->generateGrid(4, 4);

    std::jthread walker = solverThread(currentGrid, algorithmSelectionIndex, autoRunSolver);

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
            currentGrid->generateGrid(currentGrid->getMetadata().width / 2, currentGrid->getMetadata().height / 2);
        }

        if (IsKeyPressed(KEY_A)) {
            iterateRuntype--;
            runtype = static_cast<RunType>((static_cast<RunType>(iterateRuntype + 3)) % 3);
        }

        if (IsKeyPressed(KEY_D)) {
            iterateRuntype++;
            runtype = static_cast<RunType>((static_cast<RunType>(iterateRuntype + 3)) % 3);
        }

        if (IsKeyPressed(KEY_W)) {
            algorithmSelectionIndex = (algorithmSelectionIndex + 1) % solvers.size();
        }

        if (IsKeyPressed(KEY_S)) {
            algorithmSelectionIndex = (algorithmSelectionIndex + solvers.size() - 1) % solvers.size();
        }

        if (IsKeyPressed(KEY_P)) {
            autoRunSolver = !autoRunSolver;
        }

        if (IsKeyPressed(KEY_RIGHT)) {
            stepRequested.store(true, std::memory_order_relaxed);
        }

        BeginDrawing();

        ClearBackground(BLACK);
        Render::renderThread();
        currentGrid->updateTimer();

        debug(customFont, currentGrid);
        controls(customFont);
        solverstats(customFont);

        EndDrawing();

    }

    Render::unloadTexture();
    CloseWindow();
    return 0;
}
