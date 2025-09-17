//
// Created by dern on 9/15/2025.
//

#include <dansweeperml/solver/algorithm/bfsoptimized.h>
#include <dansweeperml/core/render.h>
#include <dansweeperml/core/grid.h>
#include <iostream>
#include <random>
#include <vector>

namespace algorithmbfsoptimized {

    bool BFSUnoptimized::step(Grid::Grid& grid) {
        auto meta = grid.getMetadata();
        const auto cells = grid.getCells();

        if (!started) {
            started = true;
            grid.reveal(meta.width / 2, meta.height / 2);
            Render::queueHighlightTile(meta.width / 2, meta.height / 2);

        } else {

            bool chordOrFlagged = false;

            // heuristic values if needed
            float heuristicRatio = 1.0f;
            std::pair<int, int> heuristicPair = {-1, -1};
            // get all revealed NUMBER tiles i.e. "nodes"
            for (int i = 0; i < meta.height; i++) {

                for (int j = 0; j < meta.width; j++) {

                    Grid::Cell firstPass = grid.getCellProperties(j, i);
                    if (firstPass.revealed && firstPass.adjacentMines > 0 && revealedNumberTiles.find({j, i}) == revealedNumberTiles.end()) {
                        revealedNumberTiles.insert({j, i});
                    }
                }
            }

            for (std::pair<int, int> revealedNumberTile : revealedNumberTiles) {

                if (visited.contains(revealedNumberTile)) {
                    continue;
                }

                auto [x, y] = revealedNumberTile;

                Render::queueHighlightTile(x, y);
                std::vector<std::pair<int, int>> neighbors = getNeighbors(x, y, meta.width, meta.height);
                Grid::Cell cellRevealedProperties = grid.getCellProperties(x, y);

                int unrevealedNeighbors = 0;
                int flaggedNeighbors = 0;
                int bothFlaggedAndUnrevealed = 0;

                for (std::pair<int, int> neighbor : neighbors) {

                    auto [neighborX, neighborY] = neighbor;
                    Grid::Cell cellNeighborProperties = grid.getCellProperties(neighborX, neighborY);

                    if (cellNeighborProperties.revealed == true) {
                        continue;
                    }

                    if (cellNeighborProperties.flagged == true) {
                        flaggedNeighbors++;
                    }

                    if (cellNeighborProperties.revealed == false) {
                        unrevealedNeighbors++;
                    }

                }


                if (unrevealedNeighbors == flaggedNeighbors) {
                    // only valid tiles left are flagged
                    visited.insert({x, y});
                }

                if (flaggedNeighbors == cellRevealedProperties.adjacentMines) {

                    // unnecessary chord again if all thats around is flagged == unrevealed
                    if (unrevealedNeighbors != flaggedNeighbors) {
                        grid.chord(x, y);
                        chordOrFlagged = true;
                    }

                }



                if (cellRevealedProperties.flagged && !cellRevealedProperties.revealed) {
                    bothFlaggedAndUnrevealed++;
                }

                if (unrevealedNeighbors == cellRevealedProperties.adjacentMines) {

                    for (std::pair<int, int> neighbor : neighbors) {
                        auto [flagX, flagY] = neighbor;
                        Grid::Cell cellNeighborProperties = grid.getCellProperties(flagX, flagY);
                        if (cellNeighborProperties.flagged == false && cellNeighborProperties.revealed == false) {

                            grid.flag(flagX, flagY);

                            chordOrFlagged = true;
                        }
                    }
                }



                if (!chordOrFlagged) {
                    float possibleMine = cellRevealedProperties.adjacentMines - bothFlaggedAndUnrevealed;

                    if (possibleMine > 0 && unrevealedNeighbors > 0 && possibleMine / unrevealedNeighbors < heuristicRatio) {

                        heuristicRatio = possibleMine / unrevealedNeighbors;
                        static std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
                        std::uniform_int_distribution<size_t> dist(0, neighbors.size() - 1);
                        // randomly choose so terribly

                        while (true) {

                            heuristicPair = neighbors[dist(rng)];
                            auto [heuristicX, heuristicY] = heuristicPair;
                            Grid::Cell cellHeuristicProperties = grid.getCellProperties(heuristicX, heuristicY);

                            if (cellHeuristicProperties.revealed == false && cellHeuristicProperties.flagged == false) {
                                heuristicPair = {heuristicX, heuristicY};
                                break;
                            }
                        }
                    }
                }
            }

            // fallback heuristic
            // choose best tile given lowest ratio of possible mines left / unrevealed neighbors
            if (!chordOrFlagged) {

                auto [x, y] = heuristicPair;
                grid.reveal(x, y);
                Render::queueHighlightTile(x, y);

            }

            if (cells == grid.getCells()) {
                std::cout << "failed stuck" << std::endl;
                return false;
            }

            return true;
        }

    };

    std::vector<std::pair<int, int>> BFSUnoptimized::getNeighbors(int x, int y, int width, int height) {
        std::vector<std::pair<int, int>> neighbors;

        for (int dy = -1; dy <= 1; ++dy)
            for (int dx = -1; dx <= 1; ++dx)
                if (dx != 0 || dy != 0)
                    // if out of bounds neighbor
                    if (!(x + dx < 0 || x + dx >= width || y + dy < 0 || y + dy >= height)) {
                        neighbors.emplace_back(x + dx, y + dy);
                    }

        return neighbors;
    }

    int BFSUnoptimized::getSteps() {
        return steps;
    }

    void BFSUnoptimized::reset() {
        x = 0;
        y = 0;
        steps = 0;
        visited.clear();
        revealedNumberTiles.clear();
        started = false;
    }



} // algorithmbfsunoptimized