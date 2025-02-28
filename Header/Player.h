#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <map>
#include <string>
#include "Maze.h"  // 🔹 Thêm để sử dụng Maze

class Player {
public:
    Player(SDL_Renderer* renderer);
    Player(int x, int y, SDL_Renderer* renderer);

    void handleInput(const Uint8* keys, const Maze& maze); // 🔹 Thêm Maze để kiểm tra va chạm
    void update(const Maze& maze); // 🔹 Cập nhật logic dựa trên mê cung

    void render(SDL_Renderer* renderer);
    void resetPosition(int x, int y);
    void loadKeybinds();
    ~Player();

private:
    SDL_Rect rect;
    int speed;
    float velocityY;
    bool isJumping;
    const float gravity = 0.5f;
    std::map<std::string, SDL_Keycode> keybinds;

    SDL_Texture* texture;
    SDL_Renderer* renderer;
    void loadTexture();
};

#endif
