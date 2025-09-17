//
// Created by dern on 9/7/2025.
//

#ifndef DANSWEEPER_ML_RENDER_H
#define DANSWEEPER_ML_RENDER_H

#include <raylib.h>
#include <dansweeperml/core/grid.h>
#include <chrono>

namespace Render {

    struct HighlightedTile {
        int x;
        int y;
        std::chrono::steady_clock::time_point spawn;
    };

    void loadTexture();
    void unloadTexture();
    void initializeRender(Camera2D& cam, Grid::Grid* grid);
    void renderThread();
    void queueHighlightTile(int x, int y);
    void resetHighlightTiles();

} // Render

#endif //DANSWEEPER_ML_RENDER_H