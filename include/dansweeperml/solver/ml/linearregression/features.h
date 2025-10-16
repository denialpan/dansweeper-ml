//
// Created by dern on 9/20/2025.
//

#ifndef DANSWEEPER_ML_FEATURES_H
#define DANSWEEPER_ML_FEATURES_H

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

    inline void featurize3x3(Grid::Grid& grid, int cx, int cy, std::vector<double>& out) {



    }

}

#endif //DANSWEEPER_ML_FEATURES_H