//
// Created by dern on 9/15/2025.
//

#include "../../../include/dansweeperml/solver/algorithm/linearscan.h"
#include <iostream>
#include <format>

#include "dansweeperml/core/grid.h"
#include "dansweeperml/core/render.h"


// THIS IS A TEST THAT IT MULTHREAD WITH HIGHLIGHTING WORKS
namespace algorithmlinearscan {

    bool LinearScan::step(Grid::Grid& grid) {
        x++;
        if (x > grid.getMetadata().height - 1) {
            y++;
            x = 0;
        }

        if (y > grid.getMetadata().height - 1) {
            reset();
        }

        grid.reveal(x, y);

        Render::queueHighlightTile(x, y);
        steps++;
        return true;
    }

    int LinearScan::getSteps() {
        return steps;
    }


    void LinearScan::reset() {
        x = 0;
        y = 0;
        steps = 0;
    }


} // algorithmlinearscan