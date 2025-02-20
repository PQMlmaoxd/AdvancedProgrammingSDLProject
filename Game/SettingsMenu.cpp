#include "SettingsMenu.h"
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <fstream>

SettingsMenu::SettingsMenu(SDL_Renderer* renderer) : renderer(renderer), volume(50), selectedItem(0), blinkTimer(0), blinkState(true) {
    font = TTF_OpenFont("src/fonts/arial-unicode-ms.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    }

    loadSettings();

    volumeSlider = {200, 150, 300, 10};
    volumeHandle = {200 + (volume * 3), 140, 20, 30};

    keybindRects[0] = {200, 200, 150, 40};
    keybindRects[1] = {200, 250, 150, 40};
    selectingKey[0] = selectingKey[1] = false;

    saveButton = {250, 320, 100, 40};
    draggingVolume = false;
}

SettingsMenu::~SettingsMenu() {
    TTF_CloseFont(font);
}

int SettingsMenu::run() {
    SDL_Event e;
    bool running = true;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                return -1;
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
            handleEvent(e);
        }

        render();
        SDL_Delay(16);
    }
    return 0;
}

void SettingsMenu::handleEvent(SDL_Event& e) {
    int x = e.button.x, y = e.button.y;

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (y >= volumeHandle.y && y <= volumeHandle.y + volumeHandle.h && x >= volumeSlider.x && x <= volumeSlider.x + volumeSlider.w) {
            draggingVolume = true;
        }
        if (x >= saveButton.x && x <= saveButton.x + saveButton.w && y >= saveButton.y && y <= saveButton.y + saveButton.h) {
            saveSettings();
            std::cout << "Settings saved!" << std::endl;
        }
    }

    if (e.type == SDL_MOUSEMOTION && draggingVolume) {
        if (x >= volumeSlider.x && x <= volumeSlider.x + volumeSlider.w) {
            volume = (x - volumeSlider.x) / 3;
            volumeHandle.x = volumeSlider.x + (volume * 3);
            Mix_VolumeMusic(volume * MIX_MAX_VOLUME / 100);
        }
    }

    if (e.type == SDL_MOUSEBUTTONUP) {
        draggingVolume = false;
    }

    if (e.type == SDL_KEYDOWN) {
        if (selectingKey[0]) {  
            keybinds["left"] = e.key.keysym.sym;
            selectingKey[0] = false;
            return;
        }
        if (selectingKey[1]) {  
            keybinds["right"] = e.key.keysym.sym;
            selectingKey[1] = false;
            return;
        }

        switch (e.key.keysym.sym) {
            case SDLK_UP:
                selectedItem = (selectedItem - 1 + 4) % 4;
                break;
            case SDLK_DOWN:
                selectedItem = (selectedItem + 1) % 4;
                break;
            case SDLK_LEFT:
                if (selectedItem == 0 && volume > 0) {
                    volume -= 5;
                    volumeHandle.x = volumeSlider.x + (volume * 3);
                    Mix_VolumeMusic(volume * MIX_MAX_VOLUME / 100);
                }
                break;
            case SDLK_RIGHT:
                if (selectedItem == 0 && volume < 100) {
                    volume += 5;
                    volumeHandle.x = volumeSlider.x + (volume * 3);
                    Mix_VolumeMusic(volume * MIX_MAX_VOLUME / 100);
                }
                break;
            case SDLK_RETURN:
                if (selectedItem == 1) {  // Chọn keybind trái
                    selectingKey[0] = true;
                    std::cout << "Nhấn phím mới cho di chuyển trái..." << std::endl;
                } else if (selectedItem == 2) {  // Chọn keybind phải
                    selectingKey[1] = true;
                    std::cout << "Nhấn phím mới cho di chuyển phải..." << std::endl;
                } else if (selectedItem == 3) {  // Nút Lưu
                    saveSettings();
                    std::cout << "Settings saved!" << std::endl;
                }
                break;
        }
    }
}


void SettingsMenu::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    blinkTimer++;
    if (blinkTimer >= 20) {
        blinkTimer = 0;
        blinkState = !blinkState;
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &volumeSlider);
    SDL_RenderFillRect(renderer, &volumeHandle);

    SDL_Texture* volumeText = renderText("Âm lượng:");
    SDL_Rect volumeTextRect = {50, 140, 100, 30};
    if (selectedItem == 0 && blinkState) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderDrawRect(renderer, &volumeTextRect);
    }
    SDL_RenderCopy(renderer, volumeText, NULL, &volumeTextRect);
    SDL_DestroyTexture(volumeText);

    std::string leftKeyText = selectingKey[0] ? "..." : SDL_GetKeyName(keybinds["left"]);
    SDL_Texture* keybindLeftText = renderText("Di chuyển trái: " + leftKeyText);
    SDL_Rect keybindLeftRect = {50, 200, 200, 30};
    if (selectedItem == 1 && blinkState) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderDrawRect(renderer, &keybindLeftRect);
    }
    SDL_RenderCopy(renderer, keybindLeftText, NULL, &keybindLeftRect);
    SDL_DestroyTexture(keybindLeftText);

    std::string rightKeyText = selectingKey[1] ? "..." : SDL_GetKeyName(keybinds["right"]);
    SDL_Texture* keybindRightText = renderText("Di chuyển phải: " + rightKeyText);
    SDL_Rect keybindRightRect = {50, 250, 200, 30};
    if (selectedItem == 2 && blinkState) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderDrawRect(renderer, &keybindRightRect);
    }
    SDL_RenderCopy(renderer, keybindRightText, NULL, &keybindRightRect);
    SDL_DestroyTexture(keybindRightText);

    SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);
    SDL_RenderFillRect(renderer, &saveButton);
    SDL_Texture* saveText = renderText("Lưu");
    SDL_Rect saveTextRect = {saveButton.x + 20, saveButton.y + 10, 50, 30};
    if (selectedItem == 3 && blinkState) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderDrawRect(renderer, &saveTextRect);
    }
    SDL_RenderCopy(renderer, saveText, NULL, &saveTextRect);
    SDL_DestroyTexture(saveText);

    SDL_RenderPresent(renderer);
}


void SettingsMenu::saveSettings() {
    std::ofstream file("settings.txt");
    if (file.is_open()) {
        file << "volume " << volume << std::endl;
        file << "left " << keybinds["left"] << std::endl;
        file << "right " << keybinds["right"] << std::endl;
        file.close();
    }
}

void SettingsMenu::loadSettings() {
    std::ifstream file("settings.txt");
    if (file.is_open()) {
        std::string key;
        int value;
        while (file >> key >> value) {
            if (key == "volume") {
                volume = value;
            } else {
                keybinds[key] = static_cast<SDL_Keycode>(value);
            }
        }
        file.close();
        Mix_VolumeMusic(volume * MIX_MAX_VOLUME / 100);
    } else {
        keybinds["left"] = SDLK_LEFT;
        keybinds["right"] = SDLK_RIGHT;
    }
}
SDL_Texture* SettingsMenu::renderText(const std::string& text) {
    SDL_Color color = {255, 255, 255, 255}; // Màu trắng
    SDL_Surface* surface = TTF_RenderUTF8_Solid(font, text.c_str(), color);
    if (!surface) {
        std::cerr << "Failed to render text: " << TTF_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}
