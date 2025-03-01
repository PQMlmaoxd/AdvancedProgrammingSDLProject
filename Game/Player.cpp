#include "Player.h"
#include <iostream>
#include <fstream>
#include <SDL2/SDL_image.h>
#include "Maze.h"  

Player::Player(SDL_Renderer* renderer) : renderer(renderer) {
    rect = {100, 100, 32, 32}; // 🔹 Đặt lại vị trí mặc định
    speed = 4;
    loadKeybinds();
    loadTexture();
}

Player::Player(int x, int y, SDL_Renderer* renderer) : renderer(renderer) {
    rect = {x, y, 32, 32};
    speed = 4;
    loadKeybinds();
    loadTexture();
}

// 🔹 Xử lý di chuyển (loại bỏ nhảy)
void Player::handleInput(const Uint8* keys, const Maze& maze) {
    SDL_Rect newPos = rect;

    if (keybinds.count("left") && keys[SDL_GetScancodeFromKey(keybinds["left"])]) {
        newPos.x -= speed;
    }
    if (keybinds.count("right") && keys[SDL_GetScancodeFromKey(keybinds["right"])]) {
        newPos.x += speed;
    }
    if (keybinds.count("up") && keys[SDL_GetScancodeFromKey(keybinds["up"])]) {
        newPos.y -= speed;
    }
    if (keybinds.count("down") && keys[SDL_GetScancodeFromKey(keybinds["down"])]) {
        newPos.y += speed;
    }

    // 🔹 Kiểm tra va chạm trước khi cập nhật vị trí
    if (!maze.checkCollision(newPos)) {
        rect = newPos;
    }
}

void Player::update(const Maze& maze) {
    // 🔹 Không cần xử lý trọng lực nữa
}

void Player::render(SDL_Renderer* renderer) {
    if (texture) {
        SDL_Rect renderQuad = {rect.x, rect.y, rect.w * 4, rect.h * 4};
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

    // 🔹 Nếu thiếu keybind nào, đặt mặc định là phím mũi tên
    if (keybinds.find("left") == keybinds.end()) keybinds["left"] = SDLK_LEFT;
    if (keybinds.find("right") == keybinds.end()) keybinds["right"] = SDLK_RIGHT;
    if (keybinds.find("up") == keybinds.end()) keybinds["up"] = SDLK_UP;
    if (keybinds.find("down") == keybinds.end()) keybinds["down"] = SDLK_DOWN;
}

Player::~Player() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void Player::loadTexture() {
    SDL_Surface* surface = IMG_Load("src/images/player.png");
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
