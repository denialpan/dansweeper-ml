//
// Created by dern on 9/7/2025.
//

#include "dansweeperml/grid.h"
#include "dansweeperml/render.h"

namespace Grid {

    Grid::Grid(int height, int width, int mineNum) {

        generateBoard(height, width, mineNum);

    }

    void Grid::generateBoard(int height, int width, int mineNum) {

        cells.assign(height, std::vector<Cell>(width));

        //initialize board unrevealed unflagged empty cells
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                cells[row][col].content = CELL_EMPTY;
                cells[row][col].renderTile = Tile::TILE_BLANK;
                cells[row][col].revealed = false;
                cells[row][col].flagged = false;
                cells[row][col].adjacentMines = 0;
            }
        }

        this->metadata.height = height;
        this->metadata.width = width;
        this->metadata.mineNum = mineNum;

    }

    GridMetadata Grid::getMetadata() {
        return this->metadata;
    }

    const std::vector<std::vector<Cell>>& Grid::getCells() const {
        return cells;
    }




} // Grid