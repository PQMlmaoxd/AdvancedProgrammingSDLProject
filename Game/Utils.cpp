#include "Utils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <fstream>
#include <iostream>

std::string promptForSaveName(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_StartTextInput();
    std::string saveName = "";
    bool done = false;
    SDL_Event e;

    while (!done) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                done = true;
                break;
            }
            if (e.type == SDL_TEXTINPUT) {
                saveName += e.text.text;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_BACKSPACE && !saveName.empty()) {
                    saveName.pop_back();
                }
                if (e.key.keysym.sym == SDLK_RETURN) {
                    done = true;
                }
            }
        }

        // Render giao diện nhập tên
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        std::string prompt = "Nhap ten file save: " + saveName;
        SDL_Color color = {255, 255, 255, 255};
        SDL_Surface* surface = TTF_RenderUTF8_Solid(font, prompt.c_str(), color);
        SDL_Texture* textTexture = nullptr;
        if (surface) {
            textTexture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
        }

        if (textTexture) {
            SDL_Rect rect = {50, 250, 700, 50};
            SDL_RenderCopy(renderer, textTexture, NULL, &rect);
            SDL_DestroyTexture(textTexture);
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    SDL_StopTextInput();

    // Kiểm tra tên file hợp lệ:
    if (saveName.empty()) {
        std::cout << "Ten file khong duoc de trong!\n";
        return "";
    }
    if (saveName.find('/') != std::string::npos || saveName.find('\\') != std::string::npos) {
        std::cout << "Ten file chua ky tu khong hop le!\n";
        return "";
    }
    // Kiểm tra xem file đã tồn tại hay chưa
    std::string fullPath = "Save/" + saveName + ".txt";
    std::ifstream file(fullPath);
    if (file.good()) {
        std::cout << "File da ton tai!\n";
        return "";
    }
    return saveName;
}
