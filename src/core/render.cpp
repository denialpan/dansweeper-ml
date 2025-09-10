//
// Created by dern on 9/7/2025.
//

#include "../../include/dansweeperml/render.h"

#include <raylib.h>

#include "dansweeperml/grid.h"
#include "dansweeperml/tile.h"
#include <algorithm>
#include <cmath>


namespace Render {

    static Texture2D textureTileset;
    static Camera2D camera;
    static Grid::GridMetadata meta;
    static std::vector<std::vector<Grid::Cell>> cells;

    void Render::loadTexture() {
        Image texture = LoadImage("../resources/texture.png");
        textureTileset = LoadTextureFromImage(texture);
        UnloadImage(texture);
    }

    void Render::unloadTexture() {
        UnloadTexture(textureTileset);
    }

    void initializeCamera(Grid::Grid* grid) {

        int mapWidthPixels = grid->getMetadata().width * Tile::TILE_SIZE;
        int mapHeightPixels = grid->getMetadata().height * Tile::TILE_SIZE;

        camera.target = {mapWidthPixels / 2.0f, mapHeightPixels / 2.0f};
        camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
        camera.zoom = 2.0f;
        camera.rotation = 0.0f;

        meta = grid->getMetadata();
        cells = grid->getCells();

    }

    void Render::renderThread(Grid::Grid* grid) {

        if (meta.width <= 0 || meta.height <= 0) return;

        BeginMode2D(camera);

        // world-rect of the screen (pad by 1 tile)
        const Vector2 s00 = {0, 0};
        const Vector2 s11 = {(float)GetScreenWidth(), (float)GetScreenHeight()};
        const Vector2 s01 = {0, (float)GetScreenHeight()};
        const Vector2 s10 = {(float)GetScreenWidth(), 0};

        Vector2 w00 = GetScreenToWorld2D(s00, camera);
        Vector2 w11 = GetScreenToWorld2D(s11, camera);
        Vector2 w01 = GetScreenToWorld2D(s01, camera);
        Vector2 w10 = GetScreenToWorld2D(s10, camera);

        float wxMin = std::min(std::min(w00.x, w11.x), std::min(w01.x, w10.x));
        float wxMax = std::max(std::max(w00.x, w11.x), std::max(w01.x, w10.x));
        float wyMin = std::min(std::min(w00.y, w11.y), std::min(w01.y, w10.y));
        float wyMax = std::max(std::max(w00.y, w11.y), std::max(w01.y, w10.y));

        const int pad = 1;
        const float ts = (float)Tile::TILE_SIZE;

        // convert to tile indices with floor/ceil and padding
        int startX = (int)std::floor(wxMin / ts) - pad;
        int endX   = (int)std::ceil (wxMax / ts) + pad;
        int startY = (int)std::floor(wyMin / ts) - pad;
        int endY   = (int)std::ceil (wyMax / ts) + pad;

        // clamp to grid bounds [0, width/height]
        startX = std::max(0, startX);
        startY = std::max(0, startY);
        endX   = std::min(endX,   meta.width);
        endY   = std::min(endY,   meta.height);

        // draw visible tiles
        for (int y = startY; y < endY; ++y) {
            for (int x = startX; x < endX; ++x) {
                int tileID = cells[y][x].renderTile;

                int atlasIndex = tileID;

                int srcX = (atlasIndex % Tile::TILE_ROW_COL) * Tile::TILE_SIZE;
                int srcY = (atlasIndex / Tile::TILE_ROW_COL) * Tile::TILE_SIZE;

                Rectangle src = {(float)srcX, (float)srcY, (float)Tile::TILE_SIZE, (float)Tile::TILE_SIZE};
                Vector2 pos   = {(float)(x * Tile::TILE_SIZE), (float)(y * Tile::TILE_SIZE)};
                DrawTextureRec(textureTileset, src, pos, WHITE);
            }
        }

        EndMode2D();
    }




} // Render