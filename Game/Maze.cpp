#include "Maze.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

struct Edge {
    int x1, y1, x2, y2;
};

Maze::Maze() {
    std::string latestSave = getLatestSave();
    if (!latestSave.empty() && loadMaze(latestSave)) {
        std::cout << "Loaded " << latestSave << "\n";
    } else {
        generate();
    }
}

void Maze::generate() {
    srand(time(0));

    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 20; j++) {
            maze[i][j] = 1;
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

    parent.resize(15 * 20);
    rank.resize(15 * 20, 0);
    for (int i = 0; i < 15 * 20; i++) {
        parent[i] = i;
    }

    for (const auto& e : edges) {
        int cell1 = e.y1 * 20 + e.x1;
        int cell2 = e.y2 * 20 + e.x2;

        if (findSet(cell1) != findSet(cell2)) {
            unionSets(cell1, cell2);
            maze[(e.y1 + e.y2) / 2][(e.x1 + e.x2) / 2] = 0;
        }
    }

    startX = 1;
    startY = 1;
    maze[startY][startX] = 0;

    saveMaze("Save/Save1.txt");
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
    mkdir("Save", 0777);
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "⚠️ Lỗi: Không thể lưu mê cung vào file!\n";
        return;
    }

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

std::string Maze::getLatestSave() {
    for (int i = 5; i >= 1; --i) {
        std::string filename = "Save/Save" + std::to_string(i) + ".txt";
        std::ifstream file(filename);
        if (file.good()) return filename;
    }
    return "";
}

int Maze::getStartX() const { return startX * 32; }
int Maze::getStartY() const { return startY * 32; }

bool Maze::checkCollision(const SDL_Rect& playerRect) const {
    int tileSize = 32;
    int x = playerRect.x / tileSize;
    int y = playerRect.y / tileSize;

    if (x < 0 || x >= 20 || y < 0 || y >= 15) return true;
    return maze[y][x] == 1;
}

int Maze::findSet(int v) {
    if (parent[v] == v) return v;
    return parent[v] = findSet(parent[v]);
}

void Maze::unionSets(int a, int b) {
    a = findSet(a);
    b = findSet(b);
    if (a != b) {
        if (rank[a] < rank[b])
            std::swap(a, b);
        parent[b] = a;
        if (rank[a] == rank[b])
            rank[a]++;
    }
}
