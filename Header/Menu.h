#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>
#include <string>

class Menu {
public:
    Menu(SDL_Renderer* renderer);
    ~Menu();
    int run();  // Chạy menu và chờ người dùng chọn

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Color textColor;
    std::vector<std::string> options;
    int selectedOption;
    
    // Biến xử lý nhạc
    Mix_Music* backgroundMusic;
    bool firstPlay; // Kiểm tra lần đầu chạy

    SDL_Texture* renderText(const std::string& text);
    void renderMenu();
    void playMusic();
};

#endif
