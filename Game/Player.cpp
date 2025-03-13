#include "Player.h"
#include <iostream>
#include <fstream>
#include <SDL2/SDL_image.h>
#include "Maze.h"  

Player::Player(SDL_Renderer* renderer, Maze& maze) : renderer(renderer)  {
    if (!loadPosition("save.txt")) { 
        rect = {maze.getStartX(), maze.getStartY(), 32, 32}; // Bắt đầu từ vị trí xuất phát
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
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastMoveTime < moveDelay) return;

    SDL_Rect newPos = rect;

    if (keybinds.count("left") && keys[SDL_GetScancodeFromKey(keybinds["left"])]) {
        newPos.x -= tileSize;
    }
    if (keybinds.count("right") && keys[SDL_GetScancodeFromKey(keybinds["right"])]) {
        newPos.x += tileSize;
    }
    if (keybinds.count("up") && keys[SDL_GetScancodeFromKey(keybinds["up"])]) {
        newPos.y -= tileSize;
    }
    if (keybinds.count("down") && keys[SDL_GetScancodeFromKey(keybinds["down"])]) {
        newPos.y += tileSize;
    }

    if (!maze.checkCollision(newPos)) {
        rect = newPos;
        savePosition("save.txt");
        lastMoveTime = currentTime;
    }

    // 🔹 Kiểm tra nếu đạt đích
    if (rect.x == maze.getGoalX() && rect.y == maze.getGoalY()) {
        std::cout << "🎉 Bạn đã hoàn thành màn chơi!\n";
        SDL_Delay(2000); // Dừng 2 giây trước khi thoát
        exit(0);
    }
}

void Player::update(const Maze& maze) {
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
    savePosition("save.txt");
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
