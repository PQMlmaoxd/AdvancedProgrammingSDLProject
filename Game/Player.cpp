#include "Player.h"

Player::Player() {
    rect = {100, 100, 32, 32}; // Vị trí mặc định 100,100
    speed = 4;  // Tốc độ di chuyển
}

Player::Player(int x, int y) {
    rect = {x, y, 32, 32}; // Nhân vật rộng 32x32 pixels
    speed = 4;  // Tốc độ di chuyển
}

// Xử lý input từ bàn phím
void Player::handleInput(const Uint8* keys) {
    if (keys[SDL_SCANCODE_UP])    rect.y -= speed; // Lên
    if (keys[SDL_SCANCODE_DOWN])  rect.y += speed; // Xuống
    if (keys[SDL_SCANCODE_LEFT])  rect.x -= speed; // Trái
    if (keys[SDL_SCANCODE_RIGHT]) rect.x += speed; // Phải
}

// Cập nhật logic nhân vật (có thể thêm va chạm sau này)
void Player::update() {
    // (Chưa có va chạm, chỉ di chuyển tự do)
}

// Vẽ nhân vật lên màn hình
void Player::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Màu đỏ
    SDL_RenderFillRect(renderer, &rect);
}
