#include "Key.h"
#include <SDL_image.h>
#include <iostream>

Key::Key() : keyTexture(nullptr) {
    // Đặt kích thước mặc định, ví dụ 40x40
    rect.w = 40;
    rect.h = 40;
}

Key::~Key() {
    if (keyTexture) {
        SDL_DestroyTexture(keyTexture);
    }
}

bool Key::loadTexture(SDL_Renderer* renderer, const std::string& filePath) {
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    if (!surface) {
        std::cerr << "Failed to load key image: " << IMG_GetError() << std::endl;
        return false;
    }
    keyTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!keyTexture) {
        std::cerr << "Failed to create key texture: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

void Key::setPosition(int x, int y) {
    rect.x = x;
    rect.y = y;
}

SDL_Rect Key::getRect() const {
    return rect;
}

void Key::render(SDL_Renderer* renderer) {
    if (keyTexture) {
        SDL_RenderCopy(renderer, keyTexture, NULL, &rect);  
    }
}
