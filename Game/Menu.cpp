#include "Menu.h"
#include <iostream>

Menu::Menu(SDL_Renderer* renderer) : renderer(renderer), selectedOption(0) {
    font = TTF_OpenFont("src/fonts/arial.ttf", 28);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    }

    textColor = {255, 255, 255, 255};
    options = {"Story Mode", "Endless Mode", "Guide", "Escape"};
}

Menu::~Menu() {
    TTF_CloseFont(font);
}

SDL_Texture* Menu::renderText(const std::string& text) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void Menu::renderMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (size_t i = 0; i < options.size(); i++) {
        SDL_Texture* texture = renderText(options[i]);
        int w, h;
        SDL_QueryTexture(texture, NULL, NULL, &w, &h);

        SDL_Rect rect = {300, 150 + (int)i * 50, w, h};
        if (i == selectedOption) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Màu vàng cho lựa chọn đang chọn
            SDL_RenderDrawRect(renderer, &rect);
        }

        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(renderer);
}

int Menu::run() {
    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return -1;

            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        selectedOption = (selectedOption - 1 + options.size()) % options.size();
                        break;
                    case SDLK_DOWN:
                        selectedOption = (selectedOption + 1) % options.size();
                        break;
                    case SDLK_RETURN:
                        return selectedOption; // Trả về tùy chọn đã chọn
                }
            }
        }
        renderMenu();
    }

    return -1;
}
