#include "Maze.h"
#include <cstdlib>
#include <ctime>

Maze::Maze() {
    generate();
}

void Maze::generate() {
    srand(time(0));

    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 20; j++) {
            maze[i][j] = (rand() % 3 == 0) ? 1 : 0;  // 33% tường, 66% đường
        }
    }

    maze[1][1] = 0; // Đảm bảo vị trí xuất phát không có tường
}

void Maze::render(SDL_Renderer* renderer) {
    int tileSize = 32;

    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 20; j++) {
            SDL_Rect cell = { j * tileSize, i * tileSize, tileSize, tileSize };
            if (maze[i][j] == 1) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(renderer, &cell);
            }
        }
    }
}

int Maze::getStartX() { return 1 * 32; }
int Maze::getStartY() { return 1 * 32; }

bool Maze::checkCollision(const SDL_Rect& playerRect) const {
    int tileSize = 32;
    int x = playerRect.x / tileSize;
    int y = playerRect.y / tileSize;

    if (x < 0 || x >= 20 || y < 0 || y >= 15) {
        return true;
    }

    return maze[y][x] == 1;
}
