//
// Created by dern on 9/10/2025.
//

#ifndef DANSWEEPER_ML_CONTROLLER_H
#define DANSWEEPER_ML_CONTROLLER_H
#include <raylib.h>

#include "grid.h"

namespace Controller {

    void initializeController(Camera2D& cam, Grid::Grid* g);

    void cameraZoom();
    void cameraPan();
    void cameraFocus();
    std::pair<int, int> getCoordinates();
    void cameraHover();

}

#endif //DANSWEEPER_ML_CONTROLLER_H