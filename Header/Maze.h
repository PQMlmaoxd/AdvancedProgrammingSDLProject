#ifndef MAZE_H
#define MAZE_H

#include <SDL.h>
#include <vector>

class Maze {
public:
    Maze();
    void generate();
    void render(SDL_Renderer* renderer);
    int getStartX();
    int getStartY();
    bool checkCollision(const SDL_Rect& playerRect) const; // 🔹 Kiểm tra va chạm với tường

private:
    int maze[15][20];  // 15x20 ô, 0 là đường, 1 là tường
};

#endif
