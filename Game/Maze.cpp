#include "Maze.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <direct.h>
#include <sys/types.h>

struct Edge {
    int x1, y1, x2, y2;
};

const int rows = 15;
const int cols = 20;
const int tileSize = 40;  // 🔹 Đảm bảo mỗi ô có kích thước 40px

Maze::Maze(bool forceNew) {
    if (!forceNew) {
        std::string latestSave = getLatestSave();
        if (!latestSave.empty() && loadMaze(latestSave)) {
            std::cout << "Loaded " << latestSave << "\n";
            return;
        }
    }
    generate();
}

void Maze::generate() {
    srand(time(0));

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            maze[i][j] = 1;
        }
    }

    std::vector<Edge> edges;
    for (int i = 1; i < rows; i += 2) {
        for (int j = 1; j < cols; j += 2) {
            maze[i][j] = 0;
            if (i + 2 < rows) edges.push_back({j, i, j, i + 2});
            if (j + 2 < cols) edges.push_back({j, i, j + 2, i});
        }
    }

    std::random_shuffle(edges.begin(), edges.end());

    parent.resize(rows * cols);
    rank.resize(rows * cols, 0);
    for (int i = 0; i < rows * cols; i++) {
        parent[i] = i;
    }

    for (const auto& e : edges) {
        int cell1 = e.y1 * cols + e.x1;
        int cell2 = e.y2 * cols + e.x2;

        if (findSet(cell1) != findSet(cell2)) {
            unionSets(cell1, cell2);
            maze[(e.y1 + e.y2) / 2][(e.x1 + e.x2) / 2] = 0;
        }
    }

    startX = 1;
    startY = 1;
    maze[startY][startX] = 0;

    goalX = cols - 2;
    goalY = rows - 2;
    maze[goalY][goalX] = 0;

}

void Maze::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (maze[i][j] == 1) {
                SDL_Rect cell = { j * tileSize, i * tileSize, tileSize, tileSize };
                SDL_RenderFillRect(renderer, &cell);
            }
        }
    }
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Màu xanh
    SDL_Rect goalRect = { goalX * tileSize, goalY * tileSize, tileSize, tileSize };
    SDL_RenderFillRect(renderer, &goalRect);
}

void Maze::saveMaze(const std::string& filename) {
    _mkdir("Save");
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "⚠️ Lỗi: Không thể lưu mê cung vào file!\n";
        return;
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            file << maze[i][j] << " ";
        }
        file << "\n";
    }
    file << goalX << " " << goalY << "\n";  
    file.close();
}

bool Maze::loadMaze(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            file >> maze[i][j];
        }
    }
    
    if (!(file >> goalX >> goalY)) {
        goalX = cols - 2;
        goalY = rows - 2;
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

int Maze::getStartX() const { return startX * tileSize; }
int Maze::getStartY() const { return startY * tileSize; }

bool Maze::checkCollision(const SDL_Rect& playerRect) const {
    int x1 = playerRect.x / tileSize;
    int y1 = playerRect.y / tileSize;
    int x2 = (playerRect.x + playerRect.w - 1) / tileSize;
    int y2 = (playerRect.y + playerRect.h - 1) / tileSize;

    // Nếu ra ngoài giới hạn mê cung
    if (x1 < 0 || x2 >= cols || y1 < 0 || y2 >= rows) return true;

    // Nếu vị trí mới là goal, cho phép đi qua
    if ((x1 == goalX && y1 == goalY) || 
        (x2 == goalX && y2 == goalY)) {
        return false;  // Cho phép đi vào goal
    }

    // Nếu là tường, chặn lại
    return maze[y1][x1] == 1 || maze[y1][x2] == 1 || maze[y2][x1] == 1 || maze[y2][x2] == 1;
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

int Maze::getGoalX() const { return goalX * tileSize; }
int Maze::getGoalY() const { return goalY * tileSize; }