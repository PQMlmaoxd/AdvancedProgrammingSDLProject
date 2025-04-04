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
    void update(Maze& maze, SDL_Renderer* renderer); // ⚡ Thêm renderer vào update()

    void render(SDL_Renderer* renderer);
    void resetPosition(int x, int y);
    void loadKeybinds();
    bool savePosition(const std::string& filename);
    bool loadPosition(const std::string& filename);

    ~Player();

    bool shouldReturnToMenu() const { return returnToMenu; }
    void setReturnToMenu(bool value) { returnToMenu = value; }  // ✅ Setter để cập nhật trạng thái
    int getX() const;
    int getY() const;

    void collectKey();
    bool hasKey() const;

    void setPlayTime(Uint32 time);
    Uint32 getPlayTime() const;
    void setStartTime(Uint32 time) { startTime = time; }
    SDL_Rect getRect() const;
    void setKeybind(const std::string& action, SDL_Keycode keycode);
    void updateNoKey(Maze& maze, SDL_Renderer* renderer);

    int showWinScreen1(SDL_Renderer* renderer);
    void update1(Maze& maze, SDL_Renderer* renderer);

private:
    SDL_Rect rect;
    int speed;
    std::map<std::string, SDL_Keycode> keybinds;

    SDL_Texture* textureUp;
    SDL_Texture* textureDown;
    SDL_Texture* textureSide;
    SDL_Texture* currentTexture;
    SDL_Renderer* renderer;
    void loadTexture();

    Uint32 lastMoveTime = 0;       
    const Uint32 moveDelay = 150;  
    int tileSize = 40;              

    // ⚡ Hiển thị thông báo chúc mừng ngay trên màn hình
    int showWinScreen(SDL_Renderer* renderer);

    SDL_Texture* renderText(const std::string &message, TTF_Font *font, SDL_Color color, SDL_Renderer *renderer);

    bool returnToMenu = false;
    bool facingRight = false;

    Uint32 lastFrameTime;

    bool keyCollected = false;
    
    Uint32 playTime = 0;
    Uint32 startTime;
    bool winProcessed = false;
};

#endif
