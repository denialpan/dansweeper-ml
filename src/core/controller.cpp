//
// Created by dern on 9/10/2025.
//

#include "../../include/dansweeperml/core/controller.h"

#include <algorithm>
#include <cmath>
#include <raylib.h>

namespace Controller {

    static float targetZoom = 1.0f;
    static float zoomIncrement = 0.1f;
    static Camera2D* camera = nullptr;
    static Grid::Grid* grid = nullptr;
    static Grid::GridMetadata gridMetadata;

    static std::pair<int, int> gridCoords = {-1, -1};

    std::pair<int, int> Controller::getCoordinates() {
        return gridCoords;
    }

    void Controller::initializeController(Camera2D& cam, Grid::Grid* g) {
        camera = &cam;
        grid = g;
        gridMetadata = grid->getMetadata();
    }

    void Controller::cameraZoom() {

        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {

            Vector2 mousePos = GetMousePosition();
            Vector2 worldBefore = GetWorldToScreen2D(mousePos, *camera);

            // clamp zoom
            targetZoom *= (1.0f + zoomIncrement * wheel);
            targetZoom = std::clamp(targetZoom, 1.0f, 10.0f);

            Vector2 worldAfter = GetWorldToScreen2D(mousePos, *camera);
            Vector2 diff = {
                worldAfter.x - worldBefore.x,
                worldAfter.y - worldBefore.y
            };
            camera->target = {
                camera->target.x - diff.x,
                camera->target.y - diff.y
            };

        }

        camera->zoom = std::lerp(camera->zoom, targetZoom, 0.05f);

    }

    void Controller::cameraPan() {

        if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
            Vector2 delta = GetMouseDelta();
            float scale = -1.0f / camera->zoom;
            delta.x *= scale;
            delta.y *= scale;

            camera->target.x += delta.x;
            camera->target.y += delta.y;
        }

        // clamp to window bounds
        float halfScreenWidth = GetScreenWidth() / (2.0f * camera->zoom);
        float halfScreenHeight = GetScreenHeight() / (2.0f * camera->zoom);

        int mapWidth = gridMetadata.width * Tile::TILE_SIZE;
        int mapHeight = gridMetadata.height * Tile::TILE_SIZE;

        int windowPadding = 10;

        float minX = halfScreenWidth - windowPadding;
        float maxX = mapWidth - halfScreenWidth + windowPadding;
        float minY = halfScreenHeight - windowPadding;
        float maxY = mapHeight - halfScreenHeight + windowPadding;

        if (mapWidth * camera->zoom <= GetScreenWidth()) {
            camera->target.x = mapWidth / 2.0f;
        } else {
            camera->target.x = std::clamp(camera->target.x, minX, maxX);
        }

        if (mapHeight * camera->zoom <= GetScreenHeight()) {
            camera->target.y = mapHeight / 2.0f;
        } else {
            camera->target.y = std::clamp(camera->target.y, minY, maxY);
        }

    }

    void Controller::cameraHover() {
        Vector2 world = GetScreenToWorld2D(GetMousePosition(), *camera);

        int xLocal = (int)(world.x / Tile::TILE_SIZE);
        int yLocal = (int)(world.y / Tile::TILE_SIZE);

        if (xLocal >= 0 && xLocal < gridMetadata.width && yLocal >= 0 && yLocal < gridMetadata.height) {
            gridCoords = {xLocal, yLocal};
        } else {
            gridCoords = {-1, -1};
        }
    }

    void Controller::cameraFocus() {

    }



}
