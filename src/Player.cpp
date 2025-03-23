#include "Player.h"
#include <iostream>
#include <fstream>
#include <SDL_image.h>
#include <SDL_ttf.h> // 🔹 Đảm bảo sử dụng SDL_ttf để hiển thị chữ
#include "Maze.h" 

Player::Player(SDL_Renderer* renderer, Maze& maze) : renderer(renderer) {
    if (!loadPosition("save.txt")) { 
        rect = {maze.getStartX(), maze.getStartY(), 32, 32};
    }
    loadKeybinds();
    loadTexture();
}

Player::Player(int x, int y, SDL_Renderer* renderer) : renderer(renderer) {
    lastFrameTime = SDL_GetTicks();
    rect = {x, y, 32, 32};
    speed = 275;
    loadKeybinds();
    loadTexture();
}

void Player::handleInput(const Uint8* keys, const Maze& maze) {
    // Tính delta time (thời gian giữa 2 frame)
    Uint32 currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastFrameTime) / 1000.0f;
    lastFrameTime = currentTime;

    SDL_Rect newPos = rect;
    int moveX = 0;
    int moveY = 0;

    // Kiểm tra trạng thái phím để xác định hướng di chuyển
    if (keybinds.count("left") && keys[SDL_GetScancodeFromKey(keybinds["left"])]) {
        moveX = -1;
        currentTexture = textureSide;
        facingRight = false;
    }
    if (keybinds.count("right") && keys[SDL_GetScancodeFromKey(keybinds["right"])]) {
        moveX = 1;
        currentTexture = textureSide;
        facingRight = true;
    }
    if (keybinds.count("up") && keys[SDL_GetScancodeFromKey(keybinds["up"])]) {
        moveY = -1;
        currentTexture = textureUp;
    }
    if (keybinds.count("down") && keys[SDL_GetScancodeFromKey(keybinds["down"])]) {
        moveY = 1;
        currentTexture = textureDown;
    }

    // Cập nhật vị trí dựa trên tốc độ và deltaTime (ví dụ speed = 200 pixel/giây)
    newPos.x += static_cast<int>(moveX * speed * deltaTime);
    newPos.y += static_cast<int>(moveY * speed * deltaTime);

    if (!maze.checkCollision(newPos)) {
        // Chỉ cập nhật nếu vị trí thay đổi
        if (newPos.x != rect.x || newPos.y != rect.y) {
            rect = newPos;
            // Chỉ lưu vị trí sau mỗi 500ms thay đổi, tránh lưu liên tục mỗi frame
            static Uint32 lastSaveTime = 0;
            if (currentTime - lastSaveTime >= 500) {
                savePosition("save.txt");
                lastSaveTime = currentTime;
            }
        }
    }
}

void Player::update(Maze& maze, SDL_Renderer* renderer) {
    // Kiểm tra va chạm với key (nếu chưa thu thập)
    if (maze.checkKeyCollision(rect)) {
        collectKey(); // Đánh dấu rằng player đã thu thập key
        maze.unlockDoor();
        std::cout << "Key collected!" << std::endl;
    }

    // Kiểm tra va chạm với cổng exit
    SDL_Rect goalRect = { maze.getGoalX(), maze.getGoalY(), tileSize, tileSize };
    if (SDL_HasIntersection(&rect, &goalRect)) {
        // Chỉ cho phép thoát game nếu player đã có key
        if (!hasKey()) {
            std::cout << "Exit is locked. You need a key to exit!" << std::endl;
        }
        else {
            int result = showWinScreen(renderer);
            if (result == -2) {  // Nếu chọn "Menu", đánh dấu để quay lại menu chính
                returnToMenu = true;
            }
            else if (result == -1) { // Nếu chọn "Thoát game"
                SDL_Quit();
                exit(0);
            }
        }
    }
}

int Player::showWinScreen(SDL_Renderer* renderer) {
    bool choosing = true;
    int selectedOption = 0;
    SDL_Event e;

    // Load font
    TTF_Font* font = TTF_OpenFont("resources/fonts/arial.ttf", 48);
    TTF_Font* optionFont = TTF_OpenFont("resources/fonts/arial.ttf", 28);
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
            SDL_Rect optionRect = {300, 250 + (int)i * 50, 200, 40}; // Hiển thị từ giữa màn hình xuống
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
    if (currentTexture) {
        SDL_Rect renderQuad = {rect.x, rect.y, rect.w, rect.h};
        // Nếu texture là textureSide và hướng là phải, vẽ với flip ngang
        if (currentTexture == textureSide && facingRight) {
            SDL_RenderCopyEx(renderer, currentTexture, NULL, &renderQuad, 0, NULL, SDL_FLIP_HORIZONTAL);
        } else {
            SDL_RenderCopy(renderer, currentTexture, NULL, &renderQuad);
        }
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
    if (textureUp) {
        SDL_DestroyTexture(textureUp);
    }
    if (textureDown) {
        SDL_DestroyTexture(textureDown);
    }
    if (textureSide) {
        SDL_DestroyTexture(textureSide);
    }
}


void Player::loadTexture() {
    // Khởi tạo các con trỏ về nullptr
    textureUp = nullptr;
    textureDown = nullptr;
    textureSide = nullptr;
    currentTexture = nullptr;

    SDL_Surface* surface = IMG_Load("resources/images/player_up.png");
    if (!surface) {
        std::cerr << "⚠️ Lỗi: Không thể tải ảnh nhân vật (up)! " << IMG_GetError() << std::endl;
    } else {
        textureUp = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!textureUp) {
            std::cerr << "⚠️ Lỗi: Không thể tạo texture từ ảnh (up)! " << SDL_GetError() << std::endl;
        }
    }

    surface = IMG_Load("resources/images/player_down.png");
    if (!surface) {
        std::cerr << "⚠️ Lỗi: Không thể tải ảnh nhân vật (down)! " << IMG_GetError() << std::endl;
    } else {
        textureDown = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!textureDown) {
            std::cerr << "⚠️ Lỗi: Không thể tạo texture từ ảnh (down)! " << SDL_GetError() << std::endl;
        }
    }

    surface = IMG_Load("resources/images/player_side.png");
    if (!surface) {
        std::cerr << "⚠️ Lỗi: Không thể tải ảnh nhân vật (side)! " << IMG_GetError() << std::endl;
    } else {
        textureSide = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!textureSide) {
            std::cerr << "⚠️ Lỗi: Không thể tạo texture từ ảnh (side)! " << SDL_GetError() << std::endl;
        }
    }
    
    // Chọn texture mặc định (ưu tiên textureDown, sau đó textureUp, cuối cùng textureSide)
    if (textureDown)
        currentTexture = textureDown;
    else if (textureUp)
        currentTexture = textureUp;
    else if (textureSide)
        currentTexture = textureSide;
    else
        std::cerr << "⚠️ Lỗi: Không có texture nào được load thành công!" << std::endl;
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

void Player::collectKey() {
    keyCollected = true;
}

bool Player::hasKey() const {
    return keyCollected;
}



