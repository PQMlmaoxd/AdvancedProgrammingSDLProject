#include "PauseMenu.h"
#include <iostream>

PauseMenu::PauseMenu(SDL_Renderer* renderer) : renderer(renderer), selectedOption(0) {
    font = TTF_OpenFont("src/fonts/arial.ttf", 28);
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
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
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
                        return selectedOption;
                    case SDLK_ESCAPE:
                        return 0; // Tiếp tục game
                }
            }
        }
        renderMenu();
    }
    return 0;
}
