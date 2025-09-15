//
// Created by dern on 9/7/2025.
//

#include "../../include/dansweeperml/core/grid.h"

#include <chrono>
#include <random>
#include <numeric>
#include <queue>
#include <raylib.h>

namespace Grid {

    Grid::Grid(int height, int width, int mineNum) {
        initializeEmptyGrid(height, width, mineNum);
    }

    Grid::Grid(int height, int width, float mineDensity) {
        initializeEmptyGrid(height, width, height * width * mineDensity);
    }


    // empty grid before safexy and bombs are placed
    void Grid::initializeEmptyGrid(int height, int width, int mineNum) {

        this->cells.assign(height, std::vector<Cell>(width));

        //initialize board unrevealed unflagged empty cells
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                this->cells[row][col].content = CELL_EMPTY;
                this->cells[row][col].renderTile = Tile::TILE_BLANK;
                this->cells[row][col].revealed = false;
                this->cells[row][col].flagged = false;
                this->cells[row][col].adjacentMines = 0;
            }
        }

        this->metadata.height = height;
        this->metadata.width = width;
        this->metadata.mineNum = mineNum;

    }

    // fill grid with mines from safexy and prng
    void Grid::generateGrid(int safeX, int safeY) {

        // reset grid on multiboard runs
        initializeEmptyGrid(this->metadata.height, this->metadata.width, this->metadata.mineNum);

        this->timeElapsed = 0.0f;
        this->startTime = GetTime();
        this->metadata.time = 0.0f;
        this->metadata.safeX = safeX;
        this->metadata.safeY = safeY;

        generatePrng();

        // populate grid with mines
        int totalCells = this->metadata.width * this->metadata.height;
        int safeFlat = safeY * this->metadata.width + safeX;
        std::vector<int> idx(totalCells - 1);
        std::iota(idx.begin(), idx.end(), 0);
        std::mt19937_64 gen(this->metadata.prng);
        for (int i = 0; i < this->metadata.mineNum; ++i) {
            std::uniform_int_distribution<int> dist(i, static_cast<int>(idx.size()) - 1);
            int j = dist(gen);
            std::swap(idx[i], idx[j]);

            int flat = (idx[i] >= safeFlat) ? idx[i] + 1 : idx[i];
            int y = flat / this->metadata.width, x = flat % metadata.width;
            this->cells[y][x].content = CELL_MINE;
            this->cells[y][x].renderTile = Tile::TILE_MINE_REVEALED;
        }

        // calculate adjacency tiles
        for (int y = 0; y < this->metadata.height; ++y) {
            for (int x = 0; x < this->metadata.width; ++x) {
                if (cells[y][x].content == CELL_MINE)
                    continue;

                int count = 0;
                for (int dy = -1; dy <= 1; ++dy)
                    for (int dx = -1; dx <= 1; ++dx) {
                        int nx = x + dx;
                        int ny = y + dy;
                        if (nx >= 0 && nx < this->metadata.width && ny >= 0 && ny < this->metadata.height &&
                            cells[ny][nx].content == CELL_MINE)
                            ++count;
                    }

                cells[y][x].adjacentMines = count;

                if (cells[y][x].adjacentMines == 0) {
                    cells[y][x].renderTile = Tile::TILE_BLANK;
                } else {
                    cells[y][x].renderTile = static_cast<Tile::TileId>(Tile::TILE_1 + (cells[y][x].adjacentMines - 1));
                }
            }
        }

    }

    // bfs fill reveal
    void Grid::reveal(int x, int y) {

        if (validateCoordinates(x, y)) {

            Cell& firstCell = this->cells[y][x];

            // ignore
            if (firstCell.revealed || firstCell.flagged) {
                return;
            }

            if (firstCell.content == CELL_MINE) {
                return;
            }

            std::queue<std::pair<int, int>> toReveal;
            toReveal.push({x, y});

            while (!toReveal.empty()) {
                auto [x, y] = toReveal.front();
                toReveal.pop();

                if (x < 0 || x >= this->metadata.width || y < 0 || y >= this->metadata.height)
                    continue;

                Cell& cell = cells[y][x];
                if (cell.revealed || cell.flagged)
                    continue;

                cell.revealed = true;

                if (cell.adjacentMines == 0 && cell.content != CELL_MINE) {
                    cell.renderTile = Tile::TILE_REVEALED;
                    for (int dy = -1; dy <= 1; ++dy)
                        for (int dx = -1; dx <= 1; ++dx)
                            if (dx != 0 || dy != 0)
                                toReveal.push({x + dx, y + dy});
                } else {
                    cell.renderTile = static_cast<Tile::TileId>(Tile::TILE_1 + (cell.adjacentMines - 1));
                }
            }

        }

    }

    void Grid::flag(int x, int y) {
        if (validateCoordinates(x, y)) {

        }
    }

    void Grid::chord(int x, int y) {
        if (validateCoordinates(x, y)) {

        }
    }

    void Grid::updateTimer() {
        const double now = GetTime();
        timeElapsed = static_cast<float>(now - startTime);
        metadata.time = timeElapsed;
    }

    bool Grid::validateCoordinates(int x, int y) {
        return (x >= 0 && x < this->metadata.width) && (y >= 0 && y < this->metadata.height);
    }


    void Grid::generatePrng() {

        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::system_clock::now().time_since_epoch()).count();
        std::ostringstream saltStream;
        saltStream << std::hex << ns;

        std::string salt = saltStream.str();

        std::string fullKey = std::format("{}", salt);
        std::hash<std::string> hasher;
        this->metadata.prng = hasher(fullKey);
    }


    GridMetadata Grid::getMetadata() {
        return this->metadata;
    }

    std::vector<std::vector<Cell>>& Grid::getCells() {
        return this->cells;
    }




} // Grid