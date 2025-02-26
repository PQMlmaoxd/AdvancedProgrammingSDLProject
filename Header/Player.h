#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <map>
#include <string>   

class Player {
public:
    Player(SDL_Renderer* renderer);  // 🔹 Thêm constructor nhận renderer
    Player(int x, int y, SDL_Renderer* renderer);  // 🔹 Constructor có vị trí ban đầu
    void handleInput(const Uint8* keys);
    void update();
    void render(SDL_Renderer* renderer);
    void resetPosition(int x, int y);
    void loadKeybinds();
    ~Player();  // 🔹 Giải phóng texture khi hủy đối tượng
    
private:
    SDL_Rect rect;
    int speed;
    float velocityY;
    bool isJumping;
    const float gravity = 0.5f;
    std::map<std::string, SDL_Keycode> keybinds;
    
    SDL_Texture* texture;  // 🔹 Biến texture để lưu ảnh nhân vật
    SDL_Renderer* renderer;  // 🔹 Lưu renderer để vẽ
    void loadTexture();  // 🔹 Hàm tải ảnh
};

#endif
