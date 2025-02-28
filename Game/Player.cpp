#include "Player.h"
#include <iostream>
#include <fstream>
#include <SDL2/SDL_image.h>
#include "Maze.h"  // 🔹 Thêm để dùng Maze

Player::Player(SDL_Renderer* renderer) : renderer(renderer) {
    rect = {100, 300, 32, 32};
    speed = 4;
    velocityY = 0;
    isJumping = false;
    loadKeybinds();
    loadTexture();
}

Player::Player(int x, int y, SDL_Renderer* renderer) : renderer(renderer) {
    rect = {x, y, 32, 32};
    speed = 4;
    velocityY = 0;
    isJumping = false;
    loadKeybinds();
    loadTexture();
}

// 🔹 Cập nhật để kiểm tra va chạm mê cung
void Player::handleInput(const Uint8* keys, const Maze& maze) {
    SDL_Rect newPos = rect;

    if (keybinds.count("left") && keys[SDL_GetScancodeFromKey(keybinds["left"])]) {
        newPos.x -= speed;
    }
    if (keybinds.count("right") && keys[SDL_GetScancodeFromKey(keybinds["right"])]) {
        newPos.x += speed;
    }
    if (keybinds.count("jump") && keys[SDL_GetScancodeFromKey(keybinds["jump"])]) {
        if (!isJumping) {
            velocityY = -10;
            isJumping = true;
        }
    }

    // 🔹 Kiểm tra va chạm trước khi cập nhật vị trí
    if (!maze.checkCollision(newPos)) {
        rect = newPos;
    }
}

void Player::update(const Maze& maze) {
    velocityY += gravity;
    SDL_Rect newPos = rect;
    newPos.y += (int)velocityY;

    if (!maze.checkCollision(newPos)) {
        rect = newPos;
    } else {
        velocityY = 0;
        isJumping = false;
    }
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

    if (keybinds.find("left") == keybinds.end()) keybinds["left"] = SDLK_LEFT;
    if (keybinds.find("right") == keybinds.end()) keybinds["right"] = SDLK_RIGHT;
    if (keybinds.find("jump") == keybinds.end()) keybinds["jump"] = SDLK_UP;
}

Player::~Player() {
    if (texture) {
        SDL_DestroyTexture(texture);
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

