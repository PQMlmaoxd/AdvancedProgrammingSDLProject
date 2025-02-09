#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>

class Player {
public:
    Player();  // Constructor mặc định
    Player(int x, int y);

    void handleInput(const Uint8* keys);
    void update();
    void render(SDL_Renderer* renderer);

private:
    SDL_Rect rect;
    int speed;
};

#endif
