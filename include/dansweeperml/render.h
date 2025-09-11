//
// Created by dern on 9/7/2025.
//

#ifndef DANSWEEPER_ML_RENDER_H
#define DANSWEEPER_ML_RENDER_H

#include <raylib.h>

#include "grid.h"

namespace Render {

    void loadTexture();
    void unloadTexture();
    void initializeRender(Camera2D& cam, Grid::Grid* grid);
    void renderThread();

} // Render

#endif //DANSWEEPER_ML_RENDER_H