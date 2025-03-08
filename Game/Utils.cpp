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

        // Render giao diện nhập tên trên 2 dòng:
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Dòng 1: prompt
        std::string prompt = "Nhap ten file save:";
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface* promptSurface = TTF_RenderUTF8_Solid(font, prompt.c_str(), white);
        SDL_Texture* promptTexture = SDL_CreateTextureFromSurface(renderer, promptSurface);
        SDL_FreeSurface(promptSurface);
        SDL_Rect promptRect = {50, 200, 700, 50};
        SDL_RenderCopy(renderer, promptTexture, NULL, &promptRect);
        SDL_DestroyTexture(promptTexture);

        // Dòng 2: chuỗi người dùng đã nhập
        SDL_Surface* inputSurface = TTF_RenderUTF8_Solid(font, saveName.c_str(), white);
        SDL_Texture* inputTexture = SDL_CreateTextureFromSurface(renderer, inputSurface);
        SDL_FreeSurface(inputSurface);
        SDL_Rect inputRect = {50, 260, 700, 50};
        SDL_RenderCopy(renderer, inputTexture, NULL, &inputRect);
        SDL_DestroyTexture(inputTexture);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    SDL_StopTextInput();

    // Flush các sự kiện còn lại để tránh nhận nhấn Enter thừa ở phần menu
    SDL_FlushEvent(SDL_TEXTINPUT);
    SDL_FlushEvent(SDL_KEYDOWN);

    // Kiểm tra tên file hợp lệ:
    if (saveName.empty()) {
        std::cout << "Ten file khong duoc de trong!\n";
        return "";
    }
    if (saveName.find('/') != std::string::npos || saveName.find('\\') != std::string::npos) {
        std::cout << "Ten file chua ky tu khong hop le!\n";
        return "";
    }
    // Kiểm tra xem file đã tồn tại hay chưa:
    std::string fullPath = "Save/" + saveName + ".txt";
    std::ifstream file(fullPath);
    if (file.good()) {
        std::cout << "File da ton tai!\n";
        return "";
    }
    return saveName;
}
