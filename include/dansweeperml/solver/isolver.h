//
// Created by dern on 9/14/2025.
//

#ifndef DANSWEEPER_ML_ISOLVER_H
#define DANSWEEPER_ML_ISOLVER_H

#include <dansweeperml/core/grid.h>

// every solver will take a step, private x y, amount of steps taken, and reset
class ISolver {
public:

    virtual ~ISolver() = 0;
    virtual bool step(Grid::Grid& grid) = 0;
    virtual std::string getName() = 0;
    virtual int getSteps() = 0;
    virtual void reset() = 0;

protected:
    std::string name = "";
    int x = 0;
    int y = 0;
    int steps = 0;
};

// inline desctructor
inline ISolver::~ISolver() {}

#endif //DANSWEEPER_ML_ISOLVER_H