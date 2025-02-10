#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>

class Menu {
public:
    Menu(SDL_Renderer* renderer);
    ~Menu();
    int run();  // Hiển thị menu và chờ người dùng chọn

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
