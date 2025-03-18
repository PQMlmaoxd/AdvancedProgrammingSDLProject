#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <SDL_ttf.h>  
#include <map>
#include <string>
#include "Maze.h"

class Player {
public:
    Player(SDL_Renderer* renderer, Maze& maze); 
    Player(int x, int y, SDL_Renderer* renderer);

    void handleInput(const Uint8* keys, const Maze& maze); 
    void update(const Maze& maze, SDL_Renderer* renderer); // ⚡ Thêm renderer vào update()
    void render(SDL_Renderer* renderer);
    void resetPosition(int x, int y);
    void loadKeybinds();
    bool savePosition(const std::string& filename);
    bool loadPosition(const std::string& filename);

    ~Player();

    bool shouldReturnToMenu() const { return returnToMenu; }
    void setReturnToMenu(bool value) { returnToMenu = value; }
    int getX() const;
    int getY() const;

private:
    SDL_Rect rect;
    int speed;
    std::map<std::string, SDL_Keycode> keybinds;

    // Thêm trường renderer để sử dụng trong toàn bộ lớp
    SDL_Renderer* renderer;

    // Mảng texture: 0 - UP, 1 - DOWN, 2 - SIDE; 
    // Trạng thái: 0 - idle, 1 - move
    static const int NUM_DIRECTIONS = 3;
    SDL_Texture* textures[NUM_DIRECTIONS][2];

    // Các biến dùng cho animation:
    enum Direction { UP, DOWN, SIDE };
    Direction currentDirection = DOWN; // Mặc định hướng xuống
    int currentFrame = 0;
    // Số frame của mỗi sprite sheet
    const int FRAMES_PER_STATE = 6; 
    const int frameDelay = 100;         // Delay giữa các frame (ms)
    Uint32 lastFrameTime = 0;
    
    // Biến phân biệt trạng thái: 0 - idle, 1 - move
    int currentState = 0;

    // Mảng clips: cho mỗi hướng, trạng thái và frame
    SDL_Rect clips[NUM_DIRECTIONS][2][6];

    int tileSize = 40;         

    // Hàm load texture từ file ảnh và thiết lập clip
    void loadTexture();

    // Hàm hiển thị thông báo chiến thắng
    int showWinScreen(SDL_Renderer* renderer);
    SDL_Texture* renderText(const std::string &message, TTF_Font *font, SDL_Color color, SDL_Renderer *renderer);

    Uint32 lastMoveTime = 0;       
    const Uint32 moveDelay = 150;  

    bool returnToMenu = false;
};

#endif
