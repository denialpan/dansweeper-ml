cmake_minimum_required(VERSION 4.0)
project(dansweeper_ml)

set(CMAKE_CXX_STANDARD 20)
find_package(raylib CONFIG REQUIRED)
find_path(MLPACK_INCLUDE_DIRS "mlpack.hpp")

add_executable(dansweeper_ml
        src/main.cpp
        src/core/grid.cpp
        src/core/render.cpp
        src/core/controller.cpp
        include/dansweeperml/grid.h
        include/dansweeperml/render.h
        include/dansweeperml/controller.h
        include/dansweeperml/tile.h
)

include_directories(
        ${CMAKE_SOURCE_DIR}/include
)

target_include_directories(dansweeper_ml PRIVATE ${MLPACK_INCLUDE_DIRS})
target_link_libraries(dansweeper_ml PRIVATE
    raylib
)
