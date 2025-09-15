//
// Created by dern on 9/7/2025.
//

#ifndef DANSWEEPER_ML_GRID_H
#define DANSWEEPER_ML_GRID_H

#include <vector>
#include <string>
#include <mutex>
#include <dansweeperml/core/tile.h>

namespace Grid {

    // cell literal content
    enum CellContent {
        CELL_EMPTY,
        CELL_MINE
    };

    // what defines grid
    struct GridMetadata {
        int height;
        int width;
        int mineNum;
        int prng;
        int safeX;
        int safeY;
        std::string seed64;
        float time;
    };

    enum GridState {
        SOLVED,
        UNSOLVED,
    };

    // cell interactable content
    // a way to link between renderable and literal grid content
    struct Cell {
        CellContent content = CELL_EMPTY;
        Tile::TileId renderTile = Tile::TILE_BLANK;
        bool revealed = false;
        bool flagged = false;
        int adjacentMines = 0;
    };

    class Grid {
    public:

        // manual specify number of mines, or auto with density
        Grid(int height, int width, int mineNum);
        Grid(int height, int width, float mineDensity);

        GridMetadata getMetadata();
        std::vector<std::vector<Cell>>& getCells();
        void generateGrid(int safeX, int safeY);

        void reveal(int x, int y);
        void flag(int x, int y);
        void chord(int x, int y);
        void updateTimer();



    private:

        GridMetadata metadata;
        std::vector<std::vector<Cell>> cells;

        double startTime;
        float timeElapsed;

        bool validateCoordinates(int x, int y);
        void initializeEmptyGrid(int height, int width, int mineNum);
        void generatePrng();

    };

} // Grid

#endif //DANSWEEPER_ML_GRID_H