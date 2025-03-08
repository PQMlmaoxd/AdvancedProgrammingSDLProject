#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>
#include <string>
#include "SettingsMenu.h"

class Menu {
public:
    Menu(SDL_Renderer* renderer);
    ~Menu();
    int run();  // Chạy menu chính
    int chooseGameMode(); // 🔹 Chọn Singleplayer hoặc 2 Players
    int chooseNewOrLoad(); // 🔹 Chọn New Game hoặc Load Game
    void showGuide();
    void stopMusic();
    bool selectGameMode(); // Chọn New Game hoặc Load Game
    int selectSaveSlot();  // Chọn Save Slot
    std::string chooseSaveFile(); // ✅ Khai báo chooseSaveFile
    void renderSubMenu(const std::vector<std::string>& options, int selected); // ✅ Khai báo renderSubMenu
    // Hàm getter để lấy tên file save đã chọn
    static std::string getChosenSaveFile();
    // Biến tĩnh để lưu tên file save đã chọn
    static std::string chosenSaveFile;


private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Color textColor;
    std::vector<std::string> options;
    int selectedOption;

    Mix_Music* backgroundMusic;
    bool firstPlay;

    SDL_Texture* renderText(const std::string& text, SDL_Color color);
    SDL_Texture* backgroundTexture;
    void renderMenu();
    void playMusic();
    bool confirmExit();
    void loadSettings();

    int blinkTimer;
    bool blinkState;
};

#endif
