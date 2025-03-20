#ifndef PAUSEMENU_H
#define PAUSEMENU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include "Maze.h"
#include "Player.h"

class PauseMenu {
public:
PauseMenu(SDL_Renderer* renderer, Maze& maze, Player& player);\
    ~PauseMenu();
    int run(); // Hiển thị menu pause và trả về lựa chọn
    bool confirmExit();
    void showConfirmationScreen(const std::string& message);
    Maze& maze;        // Tham chiếu đến maze
    Player& player;    // Tham chiếu đến player

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Color textColor;
    std::vector<std::string> options;
    int selectedOption;

    SDL_Texture* renderText(const std::string& text);
    void renderMenu();
    void saveGame();
};

#endif
