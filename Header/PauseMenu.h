#ifndef PAUSEMENU_H
#define PAUSEMENU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>

class PauseMenu {
public:
    PauseMenu(SDL_Renderer* renderer);
    ~PauseMenu();
    int run(); // Hiển thị menu pause và trả về lựa chọn
    bool confirmExit();
private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Color textColor;
    std::vector<std::string> options;
    int selectedOption;

    SDL_Texture* renderText(const std::string& text);
    void renderMenu();
};

#endif
