//
// Created by dern on 9/20/2025.
//

#ifndef DANSWEEPER_ML_FEATURES_H
#define DANSWEEPER_ML_FEATURES_H

#include <ranges>
#include <dansweeperml/core/grid.h>

namespace features {


    // convert board to numbers
    inline double encodeGrid(const Grid::Cell& cell) {

        // uninteracted tile
        if (!cell.revealed && !cell.flagged) {
            return -1.0;
        }

        // flagged
        if (cell.flagged) {
            return 9.0;
        }

        // return number of mines aka danger
        return cell.adjacentMines;

    }

    // featurize in 3x3 patch
    inline void featurize(Grid::Grid& grid, int cx, int cy, std::vector<double>& outputFeature) {

        const auto& cells = grid.getCells();
        const auto meta = grid.getMetadata();
        const int width = meta.width;
        const int height = meta.height;

        int unrevealed = 0;
        int flagged = 0;
        int adjacentMineSum = 0;
        int adjacentRevealed = 0;

        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {

                int x = cx + dx;
                int y = cy + dy;

                // search in bounds of grid
                if (x >= 0 && y >= 0 && x < width && y < height) {

                    const auto& checkCell = cells[y][x];

                    if (checkCell.revealed) {
                        adjacentRevealed++;
                        adjacentMineSum += checkCell.adjacentMines;
                    }

                    if (checkCell.flagged) {
                        flagged++;
                    }

                    if (!checkCell.revealed) {
                        unrevealed++;
                    }

                }

            }
        }

        double averageAdjacentMines = adjacentRevealed > 0 ? double(adjacentMineSum) / adjacentRevealed : 0.0f;

        // return features
        outputFeature.clear();
        outputFeature.push_back(double(unrevealed));
        outputFeature.push_back(double(flagged));
        outputFeature.push_back(averageAdjacentMines);
        outputFeature.push_back(adjacentRevealed);
        outputFeature.push_back(double(cx)/width);
        outputFeature.push_back(double(cy)/height);

        double ratio = (averageAdjacentMines - flagged) / std::max(1.0, double(unrevealed));
        outputFeature.push_back(ratio);

    }

}

#endif //DANSWEEPER_ML_FEATURES_H