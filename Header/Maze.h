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
    int getStartX() const; // 🔹 Đánh dấu `const` vì không thay đổi dữ liệu
    int getStartY() const;
    bool checkCollision(const SDL_Rect& playerRect) const;
    
    // 🔹 Load/Save mê cung từ file
    void saveMaze(const std::string& filename);
    bool loadMaze(const std::string& filename);

private:
    struct Edge {
        int x1, y1, x2, y2;
    };

    int maze[15][20]; // 15x20 ô, 0 là đường, 1 là tường
    int startX, startY; // 🔹 Vị trí bắt đầu của người chơi
    
    // 🔹 Các biến hỗ trợ thuật toán Kruskal
    std::vector<Edge> edges; 
    std::vector<int> parent, rank; 

    // 🔹 Hỗ trợ thuật toán Kruskal
    int findSet(int v);
    void unionSets(int a, int b);
    void initializeMaze(); // 🔹 Khởi tạo mê cung toàn tường
    std::string getLatestSave();
};

#endif
