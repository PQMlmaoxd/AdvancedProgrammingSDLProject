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

private:
    SDL_Rect rect;
    int speed;
    std::map<std::string, SDL_Keycode> keybinds;

    SDL_Texture* texture;
    SDL_Renderer* renderer;
    void loadTexture();

    Uint32 lastMoveTime = 0;       
    const Uint32 moveDelay = 150;  
    int tileSize = 40;              

    // ⚡ Hiển thị thông báo chúc mừng ngay trên màn hình
    void showWinScreen(SDL_Renderer* renderer);

    SDL_Texture* renderText(const std::string &message, TTF_Font *font, SDL_Color color, SDL_Renderer *renderer);

};

#endif
