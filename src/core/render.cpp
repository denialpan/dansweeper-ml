//
// Created by dern on 9/7/2025.
//

#include "../../include/dansweeperml/core/render.h"

#include <raylib.h>

#include "dansweeperml/core/grid.h"
#include "dansweeperml/core/tile.h"
#include <algorithm>
#include <cmath>
#include <iostream>

#include "../../include/dansweeperml/core/controller.h"


namespace Render {

    static Texture2D textureTileset;
    static Camera2D* camera = nullptr;
    static Grid::Grid* grid = nullptr;
    static Grid::GridMetadata gridMetadata;
    static std::vector<std::vector<Grid::Cell>> cells;

    void Render::loadTexture() {
        Image texture = LoadImage("../resources/texture.png");
        textureTileset = LoadTextureFromImage(texture);
        UnloadImage(texture);
    }

    void Render::unloadTexture() {
        UnloadTexture(textureTileset);
    }

    void initializeRender(Camera2D& c, Grid::Grid* g) {

        camera = &c;
        grid = g;
        cells = grid->getCells();
        gridMetadata = grid->getMetadata();

        int mapWidthPixels = grid->getMetadata().width * Tile::TILE_SIZE;
        int mapHeightPixels = grid->getMetadata().height * Tile::TILE_SIZE;

        camera->target = {mapWidthPixels / 2.0f, mapHeightPixels / 2.0f};
        camera->offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
        camera->zoom = 2.0f;
        camera->rotation = 0.0f;

    }

    void Render::renderThread() {

        if (gridMetadata.width <= 0 || gridMetadata.height <= 0) return;

        camera->offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};


        BeginMode2D(*camera);

        // CULLING: compute visible tile bounds
        Vector2 topLeft = GetScreenToWorld2D({0, 0}, *camera);
        Vector2 bottomRight = GetScreenToWorld2D(
            {(float)GetScreenWidth(), (float)GetScreenHeight()}, *camera);

        int startX = std::clamp((int)(topLeft.x / Tile::TILE_SIZE), 0, gridMetadata.width - 1);
        int endX = std::clamp((int)(bottomRight.x / Tile::TILE_SIZE) + 1, 0, gridMetadata.width);
        int startY = std::clamp((int)(topLeft.y / Tile::TILE_SIZE), 0, gridMetadata.height - 1);
        int endY = std::clamp((int)(bottomRight.y / Tile::TILE_SIZE) + 1, 0, gridMetadata.height);

        for (int y = startY; y < endY; y++) {
            for (int x = startX; x < endX; x++) {
                int tileID = cells[y][x].renderTile;
                int srcX = (tileID % Tile::TILE_ROW_COL) * Tile::TILE_SIZE;
                int srcY = (tileID / Tile::TILE_ROW_COL) * Tile::TILE_SIZE;

                Rectangle srcRect = {(float)srcX, (float)srcY, (float)Tile::TILE_SIZE, (float)Tile::TILE_SIZE};
                Vector2 pos = {(float)(x * Tile::TILE_SIZE), (float)(y * Tile::TILE_SIZE)};
                DrawTextureRec(textureTileset, srcRect, pos, WHITE);
            }
        };

        EndMode2D();
    }

} // Render