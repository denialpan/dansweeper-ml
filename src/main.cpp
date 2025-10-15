#include <raylib.h>
#include <mlpack/core.hpp>
#include <armadillo>

#include <dansweeperml/core/render.h>
#include <dansweeperml/core/controller.h>
#include <thread>
#include <memory>

#include <dansweeperml/solver/isolver.h>
#include <dansweeperml/solver/algorithm/bfsoptimized.h>
#include <dansweeperml/solver/algorithm/linearscan.h>

struct SolverStats {
    std::string name;
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

static int iterateRuntype;

std::vector<std::unique_ptr<ISolver>> solvers;
static int algorithmSelectionIndex = 2;

std::atomic<bool> stepRequested = false;
std::atomic<bool> gResetReq{false};
std::atomic<bool> gResetDone{false};
std::mutex gResetMtx;
std::condition_variable gResetCv;

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
    listOfText.push_back(std::format("name: {}", stats.name));

    for (int i = 0; i < listOfText.size(); i++) {
        DrawTextEx(font, listOfText[i].c_str(), {10, GetScreenHeight() / 2 - (15.0f * i + 20)}, 13, 1, WHITE);
    }
}

void controls(const Font &font) {

    std::vector<std::string> listOfText;

    listOfText.push_back(std::format("[->]: step forward once"));
    listOfText.push_back(std::format("[h]: toggle highlight"));
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

    listOfText.push_back(std::format("created by daniel pan"));
    listOfText.push_back(std::format("fps: {}", GetFPS()));
    listOfText.push_back(std::format("dims: {}x{}", metadata.width, metadata.height));
    listOfText.push_back(std::format("mines: {}, density: {}", metadata.mineNum, static_cast<float>(metadata.mineNum) / (metadata.width * metadata.height)));
    listOfText.push_back(std::format("prng: {}", metadata.prng));
    listOfText.push_back(std::format("safe: {}, {}", metadata.safeX, metadata.safeY));
    listOfText.push_back(std::format("time: {}", metadata.time));

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


            // request generate grid
            gResetReq.store(true, std::memory_order_release);
            {
                std::unique_lock lk(gResetMtx);
                gResetCv.wait_for(lk, std::chrono::milliseconds(100), [&] {
                    return gResetDone.load(std::memory_order_acquire) || st.stop_requested();
                });
            }

            if (st.stop_requested()) {
                return;
            }

            gResetDone.store(false, std::memory_order_release);

            Render::resetHighlightTiles();
            solver = solvers[current].get();
            solver->reset();

            stats.name = solver->getName();
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
                std::cout << "changed" << std::endl;
                resetRun();
                resetSolverStats(stats);
                solver->reset();
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
    Grid::Grid* currentGrid = new Grid::Grid(9, 9, 10);

    InitWindow(screenWidth, screenHeight, "dansweeperml");

    Camera2D camera;
    Render::initializeRender(camera, currentGrid);
    Controller::initializeController(camera, currentGrid);

    Render::loadTexture();
    Font customFont = LoadFontEx("../resources/ProggyClean.ttf", 13, 0, 250);
    SetTextureFilter(customFont.texture, TEXTURE_FILTER_POINT);

    bool drawHighlight = false;
    bool autoRunSolver = true;

    currentGrid->generateGrid(4, 4);

    std::jthread walker = solverThread(currentGrid, algorithmSelectionIndex, autoRunSolver);

    while (!WindowShouldClose()) {

        Controller::cameraZoom();
        Controller::cameraPan();
        Controller::cameraHover();

        // regenerate grid request
        if (gResetReq.exchange(false, std::memory_order_acq_rel)) {
            {
                std::unique_lock wlk(gGridMtx);
                currentGrid->generateGrid(currentGrid->getMetadata().width / 2, currentGrid->getMetadata().height / 2);
            }
            {
                std::lock_guard lk(gResetMtx);
                gResetDone.store(true, std::memory_order_release);
            }
            gResetCv.notify_all();
        }

        // debug new board
        if (IsKeyDown(KEY_SPACE)) {
            currentGrid->generateGrid(currentGrid->getMetadata().width / 2, currentGrid->getMetadata().height / 2);
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

        if (IsKeyPressed(KEY_H)) {
            drawHighlight = !drawHighlight;
        }

        BeginDrawing();

        ClearBackground(BLACK);
        Render::renderThread(drawHighlight);
        currentGrid->updateTimer();

        debug(customFont, currentGrid);
        controls(customFont);
        solverstats(customFont);

        EndDrawing();

    }


    walker.request_stop();
    if (walker.joinable()) {
        walker.join();
    }
    Render::unloadTexture();
    CloseWindow();
    return 0;
}
