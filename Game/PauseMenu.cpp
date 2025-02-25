#include "PauseMenu.h"
#include <iostream>

PauseMenu::PauseMenu(SDL_Renderer* renderer) : renderer(renderer), selectedOption(0) {
    font = TTF_OpenFont("src/fonts/arial-unicode-ms.ttf", 28);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    }

    textColor = {255, 255, 255, 255};
    options = {"Tiếp tục", "Chơi lại", "Về menu chính", "Thoát game"};
}

PauseMenu::~PauseMenu() {
    TTF_CloseFont(font);
}

SDL_Texture* PauseMenu::renderText(const std::string& text) {
    SDL_Surface* surface = TTF_RenderUTF8_Solid(font, text.c_str(), textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

bool PauseMenu::confirmExit() {
    SDL_Event e;
    bool choosing = true;
    int selected = 0; // 0 = Không, 1 = Đúng

    while (choosing) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return true;
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                    case SDLK_RIGHT:
                        selected = 1 - selected; // Chuyển giữa "Đúng" và "Không"
                        break;
                    case SDLK_RETURN:
                        return selected == 1; // Nếu chọn "Đúng", thoát game
                    case SDLK_ESCAPE:
                        return false; // Hủy thoát
                }
            }
        }

        // Hiển thị hộp thoại xác nhận
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Texture* message = renderText("Bạn thực sự muốn thoát game?");
        SDL_Rect messageRect = {200, 200, 400, 50};
        SDL_RenderCopy(renderer, message, NULL, &messageRect);
        SDL_DestroyTexture(message);

        SDL_Texture* yesText = renderText(selected == 1 ? "> Đúng <" : "Đúng");
        SDL_Texture* noText = renderText(selected == 0 ? "> Không <" : "Không");

        SDL_Rect yesRect = {250, 300, 100, 40};
        SDL_Rect noRect = {450, 300, 100, 40};

        SDL_RenderCopy(renderer, yesText, NULL, &yesRect);
        SDL_RenderCopy(renderer, noText, NULL, &noRect);

        SDL_DestroyTexture(yesText);
        SDL_DestroyTexture(noText);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    return false;
}

void PauseMenu::renderMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200); // Làm nền mờ
    SDL_RenderClear(renderer);

    for (size_t i = 0; i < options.size(); i++) {
        SDL_Texture* texture = renderText(options[i]);
        int w, h;
        SDL_QueryTexture(texture, NULL, NULL, &w, &h);

        SDL_Rect rect = {300, 200 + (int)i * 50, w, h};
        if (i == selectedOption) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Viền màu vàng
            SDL_RenderDrawRect(renderer, &rect);
        }

        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(renderer);
}

int PauseMenu::run() {
    bool paused = true;
    SDL_Event e;

    while (paused) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return 3; // Thoát game
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        selectedOption = (selectedOption - 1 + options.size()) % options.size();
                        break;
                    case SDLK_DOWN:
                        selectedOption = (selectedOption + 1) % options.size();
                        break;
                    case SDLK_RETURN:
                        if (selectedOption == 2) return -2; // Quay lại Menu chính
                        if (selectedOption == 3) { // Nếu chọn "Thoát game"
                            if (confirmExit()) {
                                SDL_Quit(); // Dọn dẹp SDL
                                exit(0);    // Thoát hoàn toàn
                            }
                        } else {
                            return selectedOption;
                        }
                        break;
                    case SDLK_ESCAPE:
                        return 0; // Tiếp tục game
                }
            }
        }
        renderMenu();
    }
    return 0;
}


