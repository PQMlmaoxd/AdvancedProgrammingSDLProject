#ifndef MAZE_H
#define MAZE_H

#include <SDL.h>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600 

class Maze {
public:
    Maze(bool forceNew = false);
    void generate(); // 🔹 Sinh mê cung bằng thuật toán Kruskal
    void render(SDL_Renderer* renderer, int playerX, int playerY);
    int getStartX() const; // 🔹 Đánh dấu `const` vì không thay đổi dữ liệu
    int getStartY() const;
    int getGoalX() const;
    int getGoalY() const;
    bool checkCollision(const SDL_Rect& playerRect) const;
    
    // 🔹 Load/Save mê cung từ file
    void saveMaze(const std::string& filename);
    bool loadMaze(const std::string& filename);

    // 🔹 Hiệu ứng bóng tối
    void createShadowMask(SDL_Renderer* renderer, int playerX, int playerY);

private:
    struct Edge {
        int x1, y1, x2, y2;
    };

    int maze[15][20]; // 15x20 ô, 0 là đường, 1 là tường
    int startX, startY;
    int goalX, goalY;
    
    // 🔹 Các biến hỗ trợ thuật toán Kruskal
    std::vector<Edge> edges; 
    std::vector<int> parent, rank; 

    // 🔹 Hỗ trợ thuật toán Kruskal
    int findSet(int v);
    void unionSets(int a, int b);
    std::string getLatestSave();

    // 🔹 Hiệu ứng bóng tối
    SDL_Texture* shadowMask = nullptr; // ✅ Thêm biến quản lý hiệu ứng bóng tối
    void drawLight(SDL_Renderer* renderer, int x, int y, int radius); // ✅ Hàm vẽ vùng sáng

};

#endif
