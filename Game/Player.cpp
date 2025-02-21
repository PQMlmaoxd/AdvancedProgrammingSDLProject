#include "Player.h"
#include <iostream>  // Thêm để debug lỗi nếu có
#include <fstream>  

Player::Player() {
    rect = {100, 300, 32, 32}; // Vị trí ban đầu
    speed = 4;
    velocityY = 0;
    isJumping = false;
    loadKeybinds();  // Nạp keybind khi tạo Player
}

Player::Player(int x, int y) {
    rect = {x, y, 32, 32};
    speed = 4;
    velocityY = 0;
    isJumping = false;
    loadKeybinds();
}

void Player::handleInput(const Uint8* keys) {
    // Đảm bảo keybind tồn tại trước khi sử dụng
    if (keybinds.count("left") && keys[SDL_GetScancodeFromKey(keybinds["left"])]) {
        rect.x -= speed;  
    }
    if (keybinds.count("right") && keys[SDL_GetScancodeFromKey(keybinds["right"])]) {
        rect.x += speed;  
    }
}

void Player::update() {
    // Áp dụng trọng lực
    velocityY += gravity;
    rect.y += (int)velocityY;

    // Giới hạn nhân vật không rơi qua mặt đất
    if (rect.y >= 300) {
        rect.y = 300;
        isJumping = false;
        velocityY = 0;
    }
}

void Player::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  
    SDL_RenderFillRect(renderer, &rect);
}

void Player::resetPosition(int x, int y) {
    rect.x = x;
    rect.y = y;
}

void Player::loadKeybinds() {
    std::ifstream file("settings.txt");

    if (!file) {  
        std::cerr << "⚠️ Lỗi: Không thể mở file settings.txt! Sử dụng mặc định.\n";
        keybinds["left"] = SDLK_LEFT;
        keybinds["right"] = SDLK_RIGHT;
        return;
    }

    std::string key;
    int value;
    while (file >> key >> value) {
        keybinds[key] = static_cast<SDL_Keycode>(value);
    }
    file.close();

    // Kiểm tra xem keybind có được đọc thành công không
    if (keybinds.find("left") == keybinds.end()) {
        std::cerr << "⚠️ Không tìm thấy keybind cho LEFT, đặt mặc định.\n";
        keybinds["left"] = SDLK_LEFT;
    }
    if (keybinds.find("right") == keybinds.end()) {
        std::cerr << "⚠️ Không tìm thấy keybind cho RIGHT, đặt mặc định.\n";
        keybinds["right"] = SDLK_RIGHT;
    }

    std::cout << "✅ Keybinds đã tải: LEFT = " << SDL_GetKeyName(keybinds["left"]) 
              << ", RIGHT = " << SDL_GetKeyName(keybinds["right"]) << "\n";
}
