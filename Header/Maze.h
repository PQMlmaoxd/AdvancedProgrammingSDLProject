#ifndef MAZE_H
#define MAZE_H

#include <SDL.h>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>

class Maze {
public:
    Maze();
    void generate(); // 🔹 Sinh mê cung bằng thuật toán Kruskal
    void render(SDL_Renderer* renderer);
    int getStartX();
    int getStartY();
    bool checkCollision(const SDL_Rect& playerRect) const;
    void saveMaze(const std::string& filename); // 🔹 Lưu mê cung vào file
    bool loadMaze(const std::string& filename); // 🔹 Tải mê cung từ file

private:
    struct Edge {
        int x1, y1, x2, y2;
    };

    int maze[15][20]; // 15x20 ô, 0 là đường, 1 là tường
    int startX, startY; // 🔹 Vị trí bắt đầu của người chơi
    std::vector<Edge> edges; // 🔹 Danh sách cạnh để hỗ trợ Kruskal
    std::vector<int> parent, rank; // 🔹 Cấu trúc dữ liệu Disjoint Set

    int findSet(int v);
    void unionSets(int a, int b);
    void initializeMaze(); // 🔹 Khởi tạo mê cung toàn tường
};

#endif
