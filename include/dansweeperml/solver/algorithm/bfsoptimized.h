//
// Created by dern on 9/15/2025.
//

#ifndef DANSWEEPER_ML_BFSUNOPTIMIZED_H
#define DANSWEEPER_ML_BFSUNOPTIMIZED_H
#include <dansweeperml/solver/isolver.h>
#include <set>

namespace algorithmbfsoptimized {

    class BFSUnoptimized : public ISolver{

    public:

        bool step(Grid::Grid& grid) override;
        int getSteps() override;
        void reset() override;
        std::string getName() override;

    protected:
        std::string name = "bfs optimized";

    private:
        std::set<std::pair<int, int>> visited;
        std::set<std::pair<int, int>> revealedNumberTiles;
        bool started = false;
        std::vector<std::pair<int, int>> getNeighbors(int x, int y, int width, int height);
    };

} // algorithmbfsunoptimized

#endif //DANSWEEPER_ML_BFSUNOPTIMIZED_H