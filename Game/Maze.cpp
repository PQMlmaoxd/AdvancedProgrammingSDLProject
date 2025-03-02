#include "Maze.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <vector>
#include <algorithm>

struct Edge {
    int x1, y1, x2, y2;
};

Maze::Maze() {
    generate();
}

void Maze::generate() {
    srand(time(0));
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 20; j++) {
            maze[i][j] = 1;  // Mặc định là tường
        }
    }
    
    std::vector<Edge> edges;
    for (int i = 1; i < 15; i += 2) {
        for (int j = 1; j < 20; j += 2) {
            maze[i][j] = 0;
            if (i + 2 < 15) edges.push_back({j, i, j, i + 2});
            if (j + 2 < 20) edges.push_back({j, i, j + 2, i});
        }
    }
    
    std::random_shuffle(edges.begin(), edges.end());
    for (const auto& e : edges) {
        int xm = (e.x1 + e.x2) / 2;
        int ym = (e.y1 + e.y2) / 2;
        maze[ym][xm] = 0;
    }
    
    maze[1][1] = 0;
    saveMaze("maze.txt");
}

void Maze::render(SDL_Renderer* renderer) {
    int tileSize = 32;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 20; j++) {
            if (maze[i][j] == 1) {
                SDL_Rect cell = { j * tileSize, i * tileSize, tileSize, tileSize };
                SDL_RenderFillRect(renderer, &cell);
            }
        }
    }
}

void Maze::saveMaze(const std::string& filename) {
    std::ofstream file(filename);
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 20; j++) {
            file << maze[i][j] << " ";
        }
        file << "\n";
    }
    file.close();
}

bool Maze::loadMaze(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 20; j++) {
            file >> maze[i][j];
        }
    }
    file.close();
    return true;
}

int Maze::getStartX() { return 1 * 32; }
int Maze::getStartY() { return 1 * 32; }

bool Maze::checkCollision(const SDL_Rect& playerRect) const {
    int tileSize = 32;
    int x = playerRect.x / tileSize;
    int y = playerRect.y / tileSize;
    if (x < 0 || x >= 20 || y < 0 || y >= 15) return true;
    return maze[y][x] == 1;
}
