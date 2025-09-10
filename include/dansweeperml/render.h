//
// Created by dern on 9/7/2025.
//

#ifndef DANSWEEPER_ML_RENDER_H
#define DANSWEEPER_ML_RENDER_H

#include "grid.h"

namespace Render {

    void loadTexture();
    void unloadTexture();
    void initializeCamera(Grid::Grid* grid);
    void renderThread(Grid::Grid* grid);

} // Render

#endif //DANSWEEPER_ML_RENDER_H