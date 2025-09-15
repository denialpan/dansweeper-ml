//
// Created by dern on 9/15/2025.
//

#ifndef DANSWEEPER_ML_LINEARSCAN_H
#define DANSWEEPER_ML_LINEARSCAN_H
#include <dansweeperml/solver/isolver.h>

#include <dansweeperml/core/grid.h>

namespace algorithmlinearscan {
    class LinearScan : public ISolver {

        void step(Grid::Grid& grid) override;
        int getSteps() override;
        void reset() override;

    };
} // algorithmlinearscan

#endif //DANSWEEPER_ML_LINEARSCAN_H