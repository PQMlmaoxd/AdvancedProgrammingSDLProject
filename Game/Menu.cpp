#include "Menu.h"
#include <iostream>
#include <fstream>

Menu::Menu(SDL_Renderer* renderer) : renderer(renderer), selectedOption(0), firstPlay(true) {
    font = TTF_OpenFont("src/fonts/arial-unicode-ms.ttf", 28);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    }

    textColor = {255, 255, 255, 255};
    options = {"Story Mode", "Endless Mode", "Guide", "Escape"};

    // Khởi tạo SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! Error: " << Mix_GetError() << std::endl;
    }

    backgroundMusic = Mix_LoadMUS("src/audio/menu_music.mp3"); // Đổi file nhạc theo ý bạn
    if (!backgroundMusic) {
        std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
    }
}

Menu::~Menu() {
    Mix_FreeMusic(backgroundMusic);
    Mix_CloseAudio();
    TTF_CloseFont(font);
}

SDL_Texture* Menu::renderText(const std::string& text) {
    SDL_Surface* surface = TTF_RenderUTF8_Solid(font, text.c_str(), textColor);
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
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderDrawRect(renderer, &rect);
        }

        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(renderer);
}

void Menu::playMusic() {
    if (!Mix_PlayingMusic()) { // Nếu nhạc chưa chạy
        Mix_PlayMusic(backgroundMusic, 1); // Chạy 1 lần đầu tiên
    }
}
void Menu::showGuide() {
    std::ifstream file("src/data/guide.txt");
    if (!file) {
        std::cerr << "Không thể mở file hướng dẫn!" << std::endl;
        return;
    }

    std::string line;
    std::vector<std::string> guideText;
    while (std::getline(file, line)) {
        guideText.push_back(line);
    }
    file.close();

    bool viewingGuide = true;
    SDL_Event e;

    while (viewingGuide) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                viewingGuide = false;
            }
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                viewingGuide = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        int yOffset = 100;
        for (const std::string& line : guideText) {
            SDL_Texture* textTexture = renderText(line);
            if (textTexture) {
                SDL_Rect textRect = {100, yOffset, 600, 30};
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
                yOffset += 40;
            }
        }

        SDL_RenderPresent(renderer);
    }
}

void Menu::stopMusic() {
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic(); // Dừng nhạc ngay lập tức
    }
}

int Menu::run() {
    bool running = true;
    SDL_Event e;

    // Phát nhạc khi menu bắt đầu
    playMusic();

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
                    if (selectedOption == 2) {  // Nếu chọn "Guide"
                        showGuide();
                    } else {
                        return selectedOption;
                    }
                }
            }
        }

        // Kiểm tra nếu nhạc kết thúc thì loop lại từ giây 193.5
        if (!Mix_PlayingMusic()) {
            Mix_PlayMusic(backgroundMusic, -1);
            Mix_SetMusicPosition(193.5);
        }

        renderMenu();
        SDL_Delay(16); // Giữ ổn định FPS
    }

    return -1;
}