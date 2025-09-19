//
// Created by dern on 9/7/2025.
//

#include "../../include/dansweeperml/core/render.h"

#include <raylib.h>

#include "dansweeperml/core/grid.h"
#include "dansweeperml/core/tile.h"
#include <algorithm>
#include <cassert>
#include <deque>

namespace Render {

    static Texture2D textureTileset;
    static Camera2D* camera = nullptr;
    static Grid::Grid* grid = nullptr;
    static Grid::GridMetadata gridMetadata;

    static std::deque<HighlightedTile> highlightedTiles;
    static std::chrono::milliseconds highlightLifetime{500};
    static size_t highlightedTilesMaxSizeTrail = 4096;
    static std::mutex highlightedTilesMtx;
    static bool drawHighlight = true;

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
        gridMetadata = grid->getMetadata();

        int mapWidthPixels = grid->getMetadata().width * Tile::TILE_SIZE;
        int mapHeightPixels = grid->getMetadata().height * Tile::TILE_SIZE;

        camera->target = {mapWidthPixels / 2.0f, mapHeightPixels / 2.0f};
        camera->offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
        camera->zoom = 2.0f;
        camera->rotation = 0.0f;

    }

    void Render::renderThread(bool drawTrail) {

        drawHighlight = drawTrail;

        if (gridMetadata.width <= 0 || gridMetadata.height <= 0) return;

        camera->offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};

        const auto& cellsRef = grid->getCells();


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
                int tileID = cellsRef[y][x].renderTile;
                int srcX = (tileID % Tile::TILE_ROW_COL) * Tile::TILE_SIZE;
                int srcY = (tileID / Tile::TILE_ROW_COL) * Tile::TILE_SIZE;

                Rectangle srcRect = {(float)srcX, (float)srcY, (float)Tile::TILE_SIZE, (float)Tile::TILE_SIZE};
                Vector2 pos = {(float)(x * Tile::TILE_SIZE), (float)(y * Tile::TILE_SIZE)};
                DrawTextureRec(textureTileset, srcRect, pos, WHITE);
            }
        };

        if (drawHighlight) {

            std::unique_lock lk(highlightedTilesMtx); // locks immediately
            std::deque<HighlightedTile> trailCopy = highlightedTiles;
            lk.unlock();

            const auto now = std::chrono::steady_clock::now();
            bool needsPrune = false;

            for (const auto& tile : trailCopy) {
                auto age = now - tile.spawn;
                if (age > highlightLifetime) { needsPrune = true; continue; }

                float t = std::clamp(std::chrono::duration<float>(age) / std::chrono::duration<float>(highlightLifetime), 0.0f, 1.0f);
                float alpha = 1.0f - t;

                assert(tile.y >= 0 && tile.y < gridMetadata.height && tile.x >= 0 && tile.x < gridMetadata.width);

                Rectangle r {
                    tile.x * (float)Tile::TILE_SIZE,
                    tile.y * (float)Tile::TILE_SIZE,
                    (float)Tile::TILE_SIZE,
                    (float)Tile::TILE_SIZE
                };

                DrawRectangleRec(r, Fade(YELLOW, alpha * 0.35f));
                DrawRectangleLinesEx(r, 1, Fade(RED, alpha * 0.35f));
            }

            // prune deque of excess tiles
            if (needsPrune) {
                std::lock_guard<std::mutex> lkClear(highlightedTilesMtx);
                const auto now2 = std::chrono::steady_clock::now();

                auto& dq = highlightedTiles;
                std::erase_if(dq, [&](const HighlightedTile& h) {
                    return (now2 - h.spawn) > highlightLifetime;
                });
            }

        }




        EndMode2D();
    }

    void queueHighlightTile(int x, int y) {

        if (!drawHighlight) {
            return;
        }

        if (x < 0 || x >= gridMetadata.width || y < 0 || y >= gridMetadata.height) {
            return;
        }
        std::lock_guard<std::mutex> lk(highlightedTilesMtx);
        if (highlightedTiles.size() >= highlightedTilesMaxSizeTrail) {
            highlightedTiles.pop_front();
        }
        highlightedTiles.push_back({x, y, std::chrono::steady_clock::now()});
    }

    void resetHighlightTiles() {
        highlightedTiles.clear();
    }


} // Render