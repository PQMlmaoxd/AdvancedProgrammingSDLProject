#include "SettingsMenu.h"
#include <SDL_mixer.h>
#include <iostream>
#include <fstream>

SettingsMenu::SettingsMenu(SDL_Renderer* renderer)
    : renderer(renderer), volume(50), selectedItem(0), blinkTimer(0), blinkState(true) {
    font = TTF_OpenFont("resources/fonts/arial-unicode-ms.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    }

    loadSettings();

    volumeSlider = {250, 150, 300, 10};
    volumeHandle = {volumeSlider.x + (volume * 3), 140, 20, 30};

    // 🔹 Khung nhập keybind (thêm lên/xuống)
    keybindRects[0] = {200, 200, 150, 40}; // Trái
    keybindRects[1] = {200, 250, 150, 40}; // Phải
    keybindRects[2] = {200, 300, 150, 40}; // Lên
    keybindRects[3] = {200, 350, 150, 40}; // Xuống
    selectingKey[0] = selectingKey[1] = selectingKey[2] = selectingKey[3] = false;

    saveButton = {250, 420, 100, 40};
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
            if (e.type == SDL_QUIT) return -1;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
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
        for (int i = 0; i < 4; i++) { // 🔹 Kiểm tra cả 4 keybind
            if (selectingKey[i]) {
                std::string keyName = (i == 0 ? "left" : (i == 1 ? "right" : (i == 2 ? "up" : "down")));
                keybinds[keyName] = e.key.keysym.sym;
                selectingKey[i] = false;
                return;
            }
        }

        switch (e.key.keysym.sym) {
            case SDLK_UP:
                selectedItem = (selectedItem - 1 + 6) % 6;  // Cập nhật số lượng mục menu
                break;
            case SDLK_DOWN:
                selectedItem = (selectedItem + 1) % 6;
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
                if (selectedItem >= 1 && selectedItem <= 4) {  // Chọn keybind
                    selectingKey[selectedItem - 1] = true;
                    std::cout << "Nhấn phím mới cho " 
                              << (selectedItem == 1 ? "di chuyển trái..." 
                              : (selectedItem == 2 ? "di chuyển phải..." 
                              : (selectedItem == 3 ? "di chuyển lên..." : "di chuyển xuống..."))) 
                              << std::endl;
                } else if (selectedItem == 5) {  // Nút Lưu
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

    SDL_Texture* volumeText = renderText("Âm lượng: " + std::to_string(volume));
    SDL_Rect volumeTextRect = {50, 140, 200, 30};
    SDL_RenderCopy(renderer, volumeText, NULL, &volumeTextRect);
    SDL_DestroyTexture(volumeText);

    // 🔹 Nếu mục "Âm Lượng" đang được chọn, vẽ viền vàng nhấp nháy
    if (selectedItem == 0 && blinkState) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderDrawRect(renderer, &volumeSlider);
    }

    std::string keyNames[] = { "left", "right", "up", "down" };
    std::string keyLabels[] = { "Di chuyển trái", "Di chuyển phải", "Di chuyển lên", "Di chuyển xuống" };

    for (int i = 0; i < 4; i++) {
        std::string keyText = selectingKey[i] ? "..." : SDL_GetKeyName(keybinds[keyNames[i]]);
        SDL_Texture* keybindText = renderText(keyLabels[i] + ": " + keyText);
        SDL_Rect keybindRect = {50, 200 + (i * 50), 250, 30};

        if (selectedItem == i + 1 && blinkState) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderDrawRect(renderer, &keybindRect);
        }
        SDL_RenderCopy(renderer, keybindText, NULL, &keybindRect);
        SDL_DestroyTexture(keybindText);
    }

    SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);
    SDL_RenderFillRect(renderer, &saveButton);
    SDL_Texture* saveText = renderText("Lưu");
    SDL_Rect saveTextRect = {saveButton.x + 20, saveButton.y + 10, 50, 30};
    SDL_RenderCopy(renderer, saveText, NULL, &saveTextRect);
    SDL_DestroyTexture(saveText);

    // 🔹 Nếu đang chọn "Lưu", vẽ viền vàng nhấp nháy
    if (selectedItem == 5 && blinkState) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderDrawRect(renderer, &saveButton);
    }

    SDL_RenderPresent(renderer);
}

void SettingsMenu::saveSettings() {
    std::ofstream file("settings.txt");
    if (file.is_open()) {
        file << "volume " << volume << std::endl;
        file << "left " << keybinds["left"] << std::endl;
        file << "right " << keybinds["right"] << std::endl;
        file << "up " << keybinds["up"] << std::endl;
        file << "down " << keybinds["down"] << std::endl;
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
                volume = value;  // 🔹 Cập nhật volume từ file
            } else {
                keybinds[key] = static_cast<SDL_Keycode>(value);
            }
        }
        volumeHandle.x = volumeSlider.x + (volume * 3);  // 🔹 Cập nhật vị trí nút điều chỉnh
        Mix_VolumeMusic(volume * MIX_MAX_VOLUME / 100);  // 🔹 Cập nhật âm lượng nhạc nền
        file.close();
        Mix_VolumeMusic(volume * MIX_MAX_VOLUME / 100);
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