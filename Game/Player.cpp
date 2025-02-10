#include "Player.h"

Player::Player() {
    rect = {100, 300, 32, 32}; // Vị trí ban đầu
    speed = 4;
    velocityY = 0;
    isJumping = false;
}

Player::Player(int x, int y) {
    rect = {x, y, 32, 32};
    speed = 4;
    velocityY = 0;
    isJumping = false;
}

void Player::handleInput(const Uint8* keys) {
    if (keys[SDL_SCANCODE_LEFT])  rect.x -= speed; // Di chuyển trái
    if (keys[SDL_SCANCODE_RIGHT]) rect.x += speed; // Di chuyển phải

    // Nhảy nếu đang ở mặt đất
    if (keys[SDL_SCANCODE_UP] && !isJumping) {
        isJumping = true;
        velocityY = -10;  // Nhảy lên
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
