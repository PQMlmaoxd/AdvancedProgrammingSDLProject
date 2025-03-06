#include "Player.h"
#include <iostream>
#include <fstream>
#include <SDL2/SDL_image.h>
#include "Maze.h"  

Player::Player(SDL_Renderer* renderer) : renderer(renderer) {
    if (!loadPosition("save.txt")) { // 🔹 Nếu không có file save, đặt vị trí mặc định
        rect = {100, 100, 32, 32};
    }
    speed = 4;
    loadKeybinds();
    loadTexture();
}

Player::Player(int x, int y, SDL_Renderer* renderer) : renderer(renderer) {
    rect = {x, y, 32, 32};
    speed = 1;
    loadKeybinds();
    loadTexture();
}

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
        savePosition("save.txt"); // 🔹 Lưu vị trí mỗi khi di chuyển hợp lệ
    }
}

void Player::update(const Maze& maze) {
    // 🔹 Không cần xử lý trọng lực nữa
}

void Player::render(SDL_Renderer* renderer) {
    if (texture) {
        SDL_Rect renderQuad = {rect.x, rect.y, rect.w, rect.h};
        SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}

void Player::resetPosition(int x, int y) {
    rect.x = x;
    rect.y = y;
    savePosition("save.txt"); // 🔹 Lưu vị trí ngay khi reset
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

bool Player::savePosition(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "⚠️ Lỗi: Không thể lưu vị trí người chơi!\n";
        return false;
    }
    file << rect.x << " " << rect.y;
    file.close();
    return true;
}

bool Player::loadPosition(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    file >> rect.x >> rect.y;
    file.close();
    return true;
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
