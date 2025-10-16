cmake_minimum_required(VERSION 4.0)
project(dansweeper_ml)

set(CMAKE_CXX_STANDARD 20)
find_package(raylib CONFIG REQUIRED)
find_package(armadillo CONFIG REQUIRED)
find_path(MLPACK_INCLUDE_DIRS "mlpack.hpp")

add_executable(dansweeper_ml
    src/main.cpp
    src/core/grid.cpp
    src/core/render.cpp
    src/core/controller.cpp

    include/dansweeperml/core/grid.h
    include/dansweeperml/core/render.h
    include/dansweeperml/core/controller.h
    include/dansweeperml/core/tile.h

    include/dansweeperml/solver/isolver.h
    include/dansweeperml/solver/algorithm/linearscan.h
    include/dansweeperml/solver/algorithm/bfsoptimized.h
    include/dansweeperml/solver/ml/linearregression/features.h

    src/solver/algorithm/linearscan.cpp
    src/solver/algorithm/bfsoptimized.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

target_include_directories(dansweeper_ml PRIVATE ${MLPACK_INCLUDE_DIRS})
target_link_libraries(dansweeper_ml PRIVATE
    raylib
    armadillo
)
