//
// Created by dern on 9/9/2025.
//

#ifndef DANSWEEPER_ML_TILE_H
#define DANSWEEPER_ML_TILE_H


namespace Tile {

    enum TileId {
        TILE_1 = 0,
        TILE_2 = 1,
        TILE_3 = 2,
        TILE_4 = 3,
        TILE_5 = 4,
        TILE_6 = 5,
        TILE_7 = 6,
        TILE_8 = 7,
        TILE_REVEALED = 8,
        TILE_BLANK = 9,
        TILE_FLAG = 10,
        TILE_MINE_WRONG = 11,
        TILE_QUESTION_REVEALED = 12,
        TILE_QUESTION = 13,
        TILE_MINE_REVEALED = 14,
        TILE_MINE_HIT = 15
    };

    const int TILE_SIZE = 16;
    const int TILE_ROW_COL = 4;

}


#endif //DANSWEEPER_ML_TILE_H