#include "Player.h"
#include <iostream>  
#include <fstream>  

Player::Player() {
    rect = {100, 300, 32, 32}; 
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
    if (keybinds.count("left") && keys[SDL_GetScancodeFromKey(keybinds["left"])]) {
        rect.x -= speed;  
    }
    if (keybinds.count("right") && keys[SDL_GetScancodeFromKey(keybinds["right"])]) {
        rect.x += speed;  
    }
    if (keybinds.count("jump") && keys[SDL_GetScancodeFromKey(keybinds["jump"])]) {
        if (!isJumping) {  // Kiểm tra nếu không nhảy mới cho nhảy
            velocityY = -10;  // Nhảy lên với vận tốc ban đầu
            isJumping = true;
        }
    }
}

void Player::update() {
    velocityY += gravity;  
    rect.y += (int)velocityY;

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
        keybinds["jump"] = SDLK_UP;  // Mặc định nhảy là phím mũi tên lên
        return;
    }

    std::string key;
    int value;
    while (file >> key >> value) {
        keybinds[key] = static_cast<SDL_Keycode>(value);
    }
    file.close();

    if (keybinds.find("left") == keybinds.end()) {
        std::cerr << "⚠️ Không tìm thấy keybind cho LEFT, đặt mặc định.\n";
        keybinds["left"] = SDLK_LEFT;
    }
    if (keybinds.find("right") == keybinds.end()) {
        std::cerr << "⚠️ Không tìm thấy keybind cho RIGHT, đặt mặc định.\n";
        keybinds["right"] = SDLK_RIGHT;
    }
    if (keybinds.find("jump") == keybinds.end()) {
        std::cerr << "⚠️ Không tìm thấy keybind cho JUMP, đặt mặc định.\n";
        keybinds["jump"] = SDLK_UP;
    }

    std::cout << "✅ Keybinds đã tải: LEFT = " << SDL_GetKeyName(keybinds["left"]) 
              << ", RIGHT = " << SDL_GetKeyName(keybinds["right"]) 
              << ", JUMP = " << SDL_GetKeyName(keybinds["jump"]) << "\n";
}
