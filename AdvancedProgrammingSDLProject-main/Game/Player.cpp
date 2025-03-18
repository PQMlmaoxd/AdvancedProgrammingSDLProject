#include "Player.h"
#include <iostream>
#include <fstream>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h> // 🔹 Đảm bảo sử dụng SDL_ttf để hiển thị chữ
#include "Maze.h" 

Player::Player(SDL_Renderer* renderer, Maze& maze) : renderer(renderer) {
    if (!loadPosition("save.txt")) { 
        rect = {maze.getStartX(), maze.getStartY(), 32, 32};
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
    bool moved = false;
    
    // Kiểm tra các phím và cập nhật vị trí, đồng thời cập nhật hướng (currentDirection)
    if (keybinds.count("left") && keys[SDL_GetScancodeFromKey(keybinds["left"])]) {
        newPos.x -= tileSize;
        currentDirection = SIDE;  // Dùng cùng một sprite cho di chuyển ngang
        moved = true;
    }
    if (keybinds.count("right") && keys[SDL_GetScancodeFromKey(keybinds["right"])]) {
        newPos.x += tileSize;
        currentDirection = SIDE;
        moved = true;
    }
    if (keybinds.count("up") && keys[SDL_GetScancodeFromKey(keybinds["up"])]) {
        newPos.y -= tileSize;
        currentDirection = UP;
        moved = true;
    }
    if (keybinds.count("down") && keys[SDL_GetScancodeFromKey(keybinds["down"])]) {
        newPos.y += tileSize;
        currentDirection = DOWN;
        moved = true;
    }

    if (moved && !maze.checkCollision(newPos)) {
        rect = newPos;
        savePosition("save.txt");
        lastMoveTime = currentTime;
        currentState = 1; // Di chuyển -> trạng thái move

        // Cập nhật frame animation dựa trên thời gian
        if (currentTime - lastFrameTime >= frameDelay) {
            currentFrame = (currentFrame + 1) % FRAMES_PER_STATE;
            lastFrameTime = currentTime;
        }
    } else {
        currentState = 0; // Không di chuyển -> trạng thái idle
        currentFrame = 0; // Hoặc giữ frame idle cố định
    }
}

void Player::update(const Maze& maze, SDL_Renderer* renderer) {
    // Kiểm tra điều kiện thắng: nếu người chơi chạm mục tiêu
    SDL_Rect goalRect = { maze.getGoalX() * tileSize, maze.getGoalY() * tileSize, tileSize, tileSize }; 
    if (SDL_HasIntersection(&rect, &goalRect)) {
        int result = showWinScreen(renderer);
        if (result == -2) {  // Nếu chọn "Menu", đánh dấu để quay lại menu chính
            returnToMenu = true;
        } else if (result == -1) { // Nếu chọn "Thoát game"
            SDL_Quit();
            exit(0);
        }
    }

    // Nếu không có di chuyển (idle), cập nhật animation idle
    // (Chú ý: trong handleInput() nếu có di chuyển, currentFrame đã được cập nhật)
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastFrameTime >= frameDelay) {
        // Nếu không di chuyển, update frame animation idle
        currentFrame = (currentFrame + 1) % FRAMES_PER_STATE;
        lastFrameTime = currentTime;
    }
}

int Player::showWinScreen(SDL_Renderer* renderer) {
    bool choosing = true;
    int selectedOption = 0;
    SDL_Event e;

    // Load font
    TTF_Font* font = TTF_OpenFont("src/fonts/arial.ttf", 48);
    TTF_Font* optionFont = TTF_OpenFont("src/fonts/arial.ttf", 28);
    SDL_Color textColor = {255, 255, 255, 255};  // Màu trắng

    while (choosing) {
        // Xóa màn hình và tô nền đen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Hiển thị "You Win!" phía trên
        SDL_Texture* winText = renderText("You Win!", font, textColor, renderer);
        SDL_Rect winRect = {300, 100, 200, 60}; // Căn giữa phía trên
        SDL_RenderCopy(renderer, winText, NULL, &winRect);
        SDL_DestroyTexture(winText);

        // Các lựa chọn phía dưới
        std::vector<std::string> options = {"Escape", "Menu"};
        for (size_t i = 0; i < options.size(); i++) {
            SDL_Color optionColor = (i == selectedOption) ? SDL_Color{255, 255, 0, 255} : textColor;
            SDL_Texture* optionText = renderText(options[i], optionFont, optionColor, renderer);
            SDL_Rect optionRect = {300, 250 + static_cast<int>(i) * 50, 200, 40}; // Hiển thị từ giữa màn hình xuống
            SDL_RenderCopy(renderer, optionText, NULL, &optionRect);
            SDL_DestroyTexture(optionText);
        }

        SDL_RenderPresent(renderer);

        // Xử lý sự kiện chọn lựa
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return -1;
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        selectedOption = (selectedOption - 1 + options.size()) % options.size();
                        break;
                    case SDLK_DOWN:
                        selectedOption = (selectedOption + 1) % options.size();
                        break;
                    case SDLK_RETURN:
                        if (selectedOption == 0) return -1; // Thoát game
                        if (selectedOption == 1) return -2; // Quay lại menu chính
                }
            }
        }
        SDL_Delay(16);
    }
    return -1;
}

void Player::render(SDL_Renderer* renderer) {
    SDL_Texture* currentTexture = textures[currentDirection][currentState];
    if (currentTexture) {
        SDL_Rect srcRect = clips[currentDirection][currentState][currentFrame];
        SDL_Rect dstRect = { rect.x, rect.y, rect.w, rect.h };
        SDL_RenderCopy(renderer, currentTexture, &srcRect, &dstRect);
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
    for (int dir = 0; dir < NUM_DIRECTIONS; ++dir) {
        for (int state = 0; state < 2; ++state) {
            if (textures[dir][state]) {
                SDL_DestroyTexture(textures[dir][state]);
                textures[dir][state] = nullptr;
            }
        }
    }
}

void Player::loadTexture() {
    // Định nghĩa các đường dẫn file cho từng hướng và trạng thái
    // Các hướng theo thứ tự: 0 - UP, 1 - DOWN, 2 - SIDE
    // Các trạng thái: 0 - idle, 1 - move
    const char* filePaths[NUM_DIRECTIONS][2] = {
        { "src/images/player_up_idle.png",   "src/images/player_up_move.png" },
        { "src/images/player_down_idle.png", "src/images/player_down_move.png" },
        { "src/images/player_side_idle.png", "src/images/player_side_move.png" }
    };

    // Duyệt qua từng hướng và trạng thái để tải texture và thiết lập clip
    for (int dir = 0; dir < NUM_DIRECTIONS; ++dir) {
        for (int state = 0; state < 2; ++state) {
            SDL_Surface* surface = IMG_Load(filePaths[dir][state]);
            if (!surface) {
                std::cerr << "Error loading " << filePaths[dir][state] 
                          << ": " << IMG_GetError() << std::endl;
                textures[dir][state] = nullptr;
                continue;
            }

            textures[dir][state] = SDL_CreateTextureFromSurface(renderer, surface);
            if (!textures[dir][state]) {
                std::cerr << "Error creating texture from " << filePaths[dir][state] 
                          << ": " << SDL_GetError() << std::endl;
            }
            
            // Giả sử mỗi ảnh PNG chứa 6 frame theo chiều ngang
            int frameCount = FRAMES_PER_STATE; // 6 frame
            int frameWidth = surface->w / frameCount;
            int frameHeight = surface->h;
            for (int frame = 0; frame < frameCount; ++frame) {
                clips[dir][state][frame].x = frame * frameWidth;
                clips[dir][state][frame].y = 0;
                clips[dir][state][frame].w = frameWidth;
                clips[dir][state][frame].h = frameHeight;
            }
            SDL_FreeSurface(surface);
        }
    }
}

SDL_Texture* Player::renderText(const std::string &message, TTF_Font *font, SDL_Color color, SDL_Renderer *renderer) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, message.c_str(), color);
    if (!surface) {
        std::cerr << "Lỗi khi tạo Surface: " << TTF_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        std::cerr << "Lỗi khi tạo Texture: " << SDL_GetError() << std::endl;
    }
    return texture;
}

int Player::getX() const { return rect.x + rect.w / 2; }
int Player::getY() const { return rect.y + rect.h / 2; }


