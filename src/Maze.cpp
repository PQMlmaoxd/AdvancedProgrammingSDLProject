#include "Maze.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <random>
#include <vector>
#include <algorithm>
#include <iostream>
#include <direct.h>
#include <sys/types.h>
#include <SDL_image.h>


struct Edge {
    int x1, y1, x2, y2;
};

const int rows = 15;
const int cols = 20;
const int tileSize = 40; 

Maze::Maze(bool forceNew) {
    if (!forceNew) {
        std::string latestSave = getLatestSave();
        if (!latestSave.empty() && loadMaze(latestSave)) {
            std::cout << "Loaded " << latestSave << "\n";
            return;
        }
    }
    generate();
}

Maze::~Maze() {
    if (wallTexture) {
        SDL_DestroyTexture(wallTexture);
    }
    if (pathTexture) {
        SDL_DestroyTexture(pathTexture);
    }
    if (doorTexture) {
        SDL_DestroyTexture(doorTexture);
    }
}

void Maze::generate() {
    srand(time(0));

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            maze[i][j] = 1;
        }
    }

    std::vector<Edge> edges;
    for (int i = 1; i < rows; i += 2) {
        for (int j = 1; j < cols; j += 2) {
            maze[i][j] = 0;
            if (i + 2 < rows) edges.push_back({j, i, j, i + 2});
            if (j + 2 < cols) edges.push_back({j, i, j + 2, i});
        }
    }

    unsigned seed = static_cast<unsigned>(time(0));
    std::default_random_engine engine(seed);
    std::shuffle(edges.begin(), edges.end(), engine);

    parent.resize(rows * cols);
    rank.resize(rows * cols, 0);
    for (int i = 0; i < rows * cols; i++) {
        parent[i] = i;
    }

    for (const auto& e : edges) {
        int cell1 = e.y1 * cols + e.x1;
        int cell2 = e.y2 * cols + e.x2;

        if (findSet(cell1) != findSet(cell2)) {
            unionSets(cell1, cell2);
            maze[(e.y1 + e.y2) / 2][(e.x1 + e.x2) / 2] = 0;
        }
    }

    startX = 1;
    startY = 1;
    maze[startY][startX] = 0;

    goalX = cols - 2;
    goalY = rows - 2;
    maze[goalY][goalX] = 0;

}

void Maze::render(SDL_Renderer* renderer, int playerX, int playerY) {
    // Nếu texture chưa được load, tải chúng
    if (!wallTexture || !pathTexture) {
        loadTextures(renderer);
    }

    // Vẽ mê cung sử dụng texture cho từng ô:
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            SDL_Rect cell = { j * tileSize, i * tileSize, tileSize, tileSize };
            if (maze[i][j] == 1) {
                // Nếu có wallTexture, dùng nó; nếu không, vẽ bằng màu trắng
                if (wallTexture) {
                    SDL_RenderCopy(renderer, wallTexture, NULL, &cell);
                }
                else {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderFillRect(renderer, &cell);
                }
            }
            else {
                // Nếu có pathTexture, dùng nó; nếu không, vẽ bằng màu đen (hoặc màu khác tùy ý)
                if (pathTexture) {
                    SDL_RenderCopy(renderer, pathTexture, NULL, &cell);
                }
                else {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderFillRect(renderer, &cell);
                }
            }
        }
    }

    // Vẽ cửa exit:
    SDL_Rect goalRect = { goalX * tileSize, goalY * tileSize, tileSize, tileSize };
    if (doorLocked) {
        // Nếu cửa khóa, kiểm tra và tải doorTexture nếu chưa có
        if (!doorTexture) {
            loadDoorTexture(renderer);
        }
        if (doorTexture) {
            SDL_RenderCopy(renderer, doorTexture, NULL, &goalRect);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
            SDL_RenderFillRect(renderer, &goalRect);
        }
    }
    else {
        // Nếu cửa mở, vẽ exit bằng màu xanh
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &goalRect);
    }

    // Vẽ key nếu chưa thu thập (key là thành viên của Maze)
    if (!keyCollected) {
        key.render(renderer);
    }

    // Tạo texture hiệu ứng bóng tối
    SDL_Texture* shadowMask = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetTextureBlendMode(shadowMask, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, shadowMask);

    // Tô màu đen toàn màn hình
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Vẽ vùng sáng xung quanh người chơi
    drawLight(renderer, playerX, playerY, 100);

    // Áp dụng hiệu ứng bóng tối lên màn hình
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, shadowMask, NULL, NULL);
    SDL_DestroyTexture(shadowMask);
}

void Maze::drawLight(SDL_Renderer* renderer, int x, int y, int radius) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180); // Màu đen nhưng có độ trong suốt

    for (int w = -radius; w <= radius; w++) {
        for (int h = -radius; h <= radius; h++) {
            int dx = w;
            int dy = h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

void Maze::saveMaze(const std::string& filename) {
    _mkdir("Save");
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "⚠️ Lỗi: Không thể lưu mê cung vào file!\n";
        return;
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            file << maze[i][j] << " ";
        }
        file << "\n";
    }
    file << goalX << " " << goalY << "\n";  
    file.close();
}

bool Maze::loadMaze(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            file >> maze[i][j];
        }
    }
    
    if (!(file >> goalX >> goalY)) {
        goalX = cols - 2;
        goalY = rows - 2;
    }
    
    file.close();
    return true;
}

std::string Maze::getLatestSave() {
    for (int i = 5; i >= 1; --i) {
        std::string filename = "Save/Save" + std::to_string(i) + ".txt";
        std::ifstream file(filename);
        if (file.good()) return filename;
    }
    return "";
}

int Maze::getStartX() const { return startX * tileSize; }
int Maze::getStartY() const { return startY * tileSize; }

bool Maze::checkCollision(const SDL_Rect& playerRect) const {
    int x1 = playerRect.x / tileSize;
    int y1 = playerRect.y / tileSize;
    int x2 = (playerRect.x + playerRect.w - 1) / tileSize;
    int y2 = (playerRect.y + playerRect.h - 1) / tileSize;

    // Nếu ra ngoài giới hạn mê cung
    if (x1 < 0 || x2 >= cols || y1 < 0 || y2 >= rows) return true;

    // Nếu vị trí mới là goal, cho phép đi qua
    if ((x1 == goalX && y1 == goalY) || 
        (x2 == goalX && y2 == goalY)) {
        return false;  // Cho phép đi vào goal
    }

    // Nếu là tường, chặn lại
    return maze[y1][x1] == 1 || maze[y1][x2] == 1 || maze[y2][x1] == 1 || maze[y2][x2] == 1;
}

int Maze::findSet(int v) {
    if (parent[v] == v) return v;
    return parent[v] = findSet(parent[v]);
}

void Maze::unionSets(int a, int b) {
    a = findSet(a);
    b = findSet(b);
    if (a != b) {
        if (rank[a] < rank[b])
            std::swap(a, b);
        parent[b] = a;
        if (rank[a] == rank[b])
            rank[a]++;
    }
}

int Maze::getGoalX() const { return goalX * tileSize; }
int Maze::getGoalY() const { return goalY * tileSize; }

void Maze::createShadowMask(SDL_Renderer* renderer, int playerX, int playerY) {
    if (shadowMask) SDL_DestroyTexture(shadowMask);

    // Tạo texture đen phủ toàn bộ màn hình
    shadowMask = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderTarget(renderer, shadowMask);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Vẽ vùng sáng xung quanh người chơi
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    drawLight(renderer, playerX, playerY, 100);

    SDL_SetRenderTarget(renderer, NULL);
}

void Maze::spawnKey(SDL_Renderer* renderer) {
    // Tạo vị trí ngẫu nhiên trong mê cung
    // Ví dụ: đảm bảo khoảng cách từ spawn (startX, startY) > 3 ô
    int keyX, keyY;
    do {
        keyX = (rand() % cols);
        keyY = (rand() % rows);
    } while ((abs(keyX - startX) < 3 && abs(keyY - startY) < 3) || maze[keyY][keyX] != 0);

    // Đặt vị trí key theo pixel (nếu mỗi ô có kích thước tileSize)
    key.setPosition(keyX * tileSize, keyY * tileSize);

    // Tải texture cho key (ví dụ: "resources/images/key.png")
    if (!key.loadTexture(renderer, "resources/images/key.png")) {
        std::cerr << "Failed to load key texture" << std::endl;
    }
}

bool Maze::checkKeyCollision(const SDL_Rect& playerRect) {
    // Nếu key chưa được thu thập
    if (!keyCollected) {
        SDL_Rect keyRect = key.getRect();
        // Nếu có va chạm giữa player và key
        if (SDL_HasIntersection(&playerRect, &keyRect)) {
            keyCollected = true;
            std::cout << "Player collected the key!" << std::endl;
            return true;
        }
    }
    return false;
}

void Maze::loadDoorTexture(SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load("resources/images/door_locked.png");
    if (!surface) {
        std::cerr << "Failed to load door texture: " << IMG_GetError() << std::endl;
        return;
    }
    doorTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!doorTexture) {
        std::cerr << "Failed to create door texture: " << SDL_GetError() << std::endl;
    }
}

void Maze::unlockDoor() {
    doorLocked = false;
    // Nếu muốn, có thể thay đổi tile tại exit thành 0 để không gây va chạm nữa:
    maze[goalY][goalX] = 0;
}

void Maze::loadTextures(SDL_Renderer* renderer) {
    SDL_Surface* wallSurface = IMG_Load("resources/images/wall.png");
    if (!wallSurface) {
        std::cerr << "Failed to load wall image: " << IMG_GetError() << std::endl;
    }
    else {
        wallTexture = SDL_CreateTextureFromSurface(renderer, wallSurface);
        SDL_FreeSurface(wallSurface);
    }

    SDL_Surface* pathSurface = IMG_Load("resources/images/path.png");
    if (!pathSurface) {
        std::cerr << "Failed to load path image: " << IMG_GetError() << std::endl;
    }
    else {
        pathTexture = SDL_CreateTextureFromSurface(renderer, pathSurface);
        SDL_FreeSurface(pathSurface);
    }
}




