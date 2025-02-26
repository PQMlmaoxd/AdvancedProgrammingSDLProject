#include "Player.h"
#include <iostream>  
#include <fstream>  
#include <SDL2/SDL_image.h>

Player::Player(SDL_Renderer* renderer) : renderer(renderer) { // 🔹 Truyền renderer vào
    rect = {100, 300, 32, 32}; 
    speed = 4;
    velocityY = 0;
    isJumping = false;
    loadKeybinds();  // Nạp keybind khi tạo Player
    loadTexture(); // 🔹 Tải ảnh nhân vật
}

Player::Player(int x, int y, SDL_Renderer* renderer) : renderer(renderer) { // 🔹 Truyền renderer vào
    rect = {x, y, 32, 32};
    speed = 4;
    velocityY = 0;
    isJumping = false;
    loadKeybinds();
    loadTexture(); // 🔹 Tải ảnh nhân vật
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

void Player::loadTexture() {
    SDL_Surface* surface = IMG_Load("src/images/player.png");  // 🔹 Đổi đường dẫn ảnh tại đây
    if (!surface) {
        std::cerr << "⚠️ Lỗi: Không thể tải ảnh nhân vật! " << IMG_GetError() << std::endl;
        texture = nullptr;
        return;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        std::cerr << "⚠️ Lỗi: Không thể tạo texture từ ảnh! " << SDL_GetError() << std::endl;
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
    if (texture) { 
        SDL_Rect renderQuad = {rect.x, rect.y, rect.w * 4, rect.h * 4}; // 🔹 Nhân đôi kích thước
        SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
    } else { 
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  
        SDL_RenderFillRect(renderer, &rect);
    }
}


void Player::resetPosition(int x, int y) {
    rect.x = x;
    rect.y = y;
}

void Player::loadKeybinds() {
    std::ifstream file("settings.txt");
    if (file.is_open()) {
        std::string key;
        int value;
        while (file >> key >> value) {
            keybinds[key] = static_cast<SDL_Keycode>(value);
        }
        file.close();
    }

    // Nếu thiếu keybind nào, đặt mặc định
    if (keybinds.find("left") == keybinds.end()) keybinds["left"] = SDLK_LEFT;
    if (keybinds.find("right") == keybinds.end()) keybinds["right"] = SDLK_RIGHT;
    if (keybinds.find("jump") == keybinds.end()) keybinds["jump"] = SDLK_UP;

    std::cout << "🎮 Keybinds: LEFT=" << SDL_GetKeyName(keybinds["left"]) 
              << ", RIGHT=" << SDL_GetKeyName(keybinds["right"]) 
              << ", JUMP=" << SDL_GetKeyName(keybinds["jump"]) << "\n";
}

Player::~Player() {
    if (texture) {
        SDL_DestroyTexture(texture); // 🔹 Giải phóng bộ nhớ
    }
}
