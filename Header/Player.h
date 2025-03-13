#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <map>
#include <string>
#include "Maze.h"  // 🔹 Thêm để sử dụng Maze

class Player {
public:
    Player(SDL_Renderer* renderer, Maze& maze); 
    Player(int x, int y, SDL_Renderer* renderer);

    void handleInput(const Uint8* keys, const Maze& maze); // 🔹 Kiểm tra va chạm với mê cung
    void update(const Maze& maze); // 🔹 Cập nhật logic di chuyển

    void render(SDL_Renderer* renderer);
    void resetPosition(int x, int y);
    void loadKeybinds();
    bool savePosition(const std::string& filename); // 🔹 Lưu vị trí người chơi
    bool loadPosition(const std::string& filename); // 🔹 Tải vị trí từ file save

    ~Player();

private:
    SDL_Rect rect;
    int speed;
    std::map<std::string, SDL_Keycode> keybinds;

    SDL_Texture* texture;
    SDL_Renderer* renderer;
    void loadTexture();

    Uint32 lastMoveTime = 0;           // Thời gian di chuyển cuối cùng
    const Uint32 moveDelay = 150;      // Delay 150 ms giữa các lần di chuyển
    int tileSize = 40;                 // Nếu tileSize của maze là 32, thay đổi nếu cần

};

#endif
