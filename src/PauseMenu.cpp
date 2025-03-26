#include "PauseMenu.h"
#include <iostream>
#include "Menu.h"

PauseMenu::PauseMenu(SDL_Renderer* renderer, Maze& maze, Player& player)
: renderer(renderer), maze(maze), player(player), selectedOption(0) {
font = TTF_OpenFont("resources/fonts/arial-unicode-ms.ttf", 28);
if (!font) {
    std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
}
textColor = {255, 255, 255, 255};
options = {"Tiếp tục", "Chơi lại", "Lưu game", "Về menu chính", "Thoát game"};
}

bool PauseMenu::confirmconfirmSaveGame() {
    SDL_Event e;
    bool choosing = true;
    int selected = 0; // 0 = Không, 1 = Có

    while (choosing) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return false;
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_LEFT:
                case SDLK_RIGHT:
                    selected = 1 - selected;
                    break;
                case SDLK_RETURN:
                    return selected == 1;
                case SDLK_ESCAPE:
                    return false;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render câu hỏi xác nhận lưu game
        SDL_Surface* messageSurface = TTF_RenderUTF8_Blended(font, "Bạn có muốn lưu game không?", { 255, 255, 255, 255 });
        SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(renderer, messageSurface);
        SDL_Rect messageRect = { (800 - messageSurface->w) / 2, 200, messageSurface->w, messageSurface->h };
        SDL_FreeSurface(messageSurface);
        SDL_RenderCopy(renderer, messageTexture, NULL, &messageRect);
        SDL_DestroyTexture(messageTexture);

        // Render lựa chọn "Có"
        SDL_Surface* yesSurface = TTF_RenderUTF8_Blended(font, (selected == 1 ? "> Có <" : "Có"), { 255, 255, 255, 255 });
        SDL_Texture* yesTexture = SDL_CreateTextureFromSurface(renderer, yesSurface);
        SDL_Rect yesRect = { 250, 300, yesSurface->w, yesSurface->h };
        SDL_FreeSurface(yesSurface);
        SDL_RenderCopy(renderer, yesTexture, NULL, &yesRect);
        SDL_DestroyTexture(yesTexture);

        // Render lựa chọn "Không"
        SDL_Surface* noSurface = TTF_RenderUTF8_Blended(font, (selected == 0 ? "> Không <" : "Không"), { 255, 255, 255, 255 });
        SDL_Texture* noTexture = SDL_CreateTextureFromSurface(renderer, noSurface);
        SDL_Rect noRect = { 450, 300, noSurface->w, noSurface->h };
        SDL_FreeSurface(noSurface);
        SDL_RenderCopy(renderer, noTexture, NULL, &noRect);
        SDL_DestroyTexture(noTexture);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    return false;
}

PauseMenu::~PauseMenu() {
    TTF_CloseFont(font);
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
                        selected = 1 - selected;
                        break;
                    case SDLK_RETURN:
                        return selected == 1;
                    case SDLK_ESCAPE:
                        return false;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render câu hỏi xác nhận thoát game
        SDL_Surface* messageSurface = TTF_RenderUTF8_Blended(font, "Bạn thực sự muốn thoát game?", { 255, 255, 255, 255 });
        SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(renderer, messageSurface);
        SDL_Rect messageRect = { (800 - messageSurface->w) / 2, 200, messageSurface->w, messageSurface->h };
        SDL_FreeSurface(messageSurface);
        SDL_RenderCopy(renderer, messageTexture, NULL, &messageRect);
        SDL_DestroyTexture(messageTexture);

        // Render lựa chọn "Đúng"
        SDL_Surface* yesSurface = TTF_RenderUTF8_Blended(font, (selected == 1 ? "> Đúng <" : "Đúng"), { 255, 255, 255, 255 });
        SDL_Texture* yesTexture = SDL_CreateTextureFromSurface(renderer, yesSurface);
        SDL_Rect yesRect = { 250, 300, yesSurface->w, yesSurface->h };
        SDL_FreeSurface(yesSurface);
        SDL_RenderCopy(renderer, yesTexture, NULL, &yesRect);
        SDL_DestroyTexture(yesTexture);

        // Render lựa chọn "Không"
        SDL_Surface* noSurface = TTF_RenderUTF8_Blended(font, (selected == 0 ? "> Không <" : "Không"), { 255, 255, 255, 255 });
        SDL_Texture* noTexture = SDL_CreateTextureFromSurface(renderer, noSurface);
        SDL_Rect noRect = { 450, 300, noSurface->w, noSurface->h };
        SDL_FreeSurface(noSurface);
        SDL_RenderCopy(renderer, noTexture, NULL, &noRect);
        SDL_DestroyTexture(noTexture);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    return false;
}

void PauseMenu::renderMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200); // Nền mờ
    SDL_RenderClear(renderer);

    for (size_t i = 0; i < options.size(); i++) {
        // Tạo surface từ text
        SDL_Surface* surface = TTF_RenderUTF8_Blended(font, options[i].c_str(), { 255, 255, 255, 255 });
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        // Lấy kích thước chữ để căn giữa
        int w = surface->w;
        int h = surface->h;
        SDL_Rect rect = { (800 - w) / 2, 200 + (int)i * 50, w, h }; // Căn giữa màn hình

        SDL_FreeSurface(surface); // Giải phóng surface ngay sau khi tạo texture

        if (i == selectedOption) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Viền vàng cho mục được chọn
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
            if (e.type == SDL_QUIT) return 3;
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                    selectedOption = (selectedOption - 1 + options.size()) % options.size();
                    break;
                case SDLK_DOWN:
                    selectedOption = (selectedOption + 1) % options.size();
                    break;
                case SDLK_RETURN:
                    if (selectedOption == 0) return 0;      // Tiếp tục game
                    if (selectedOption == 1) return 1;      // Chơi lại
                    if (selectedOption == 2) {              // Lưu game
                        saveGame();
                        return 0;
                    }
                    if (selectedOption == 3) { // Về menu chính
                        if (confirmconfirmSaveGame()) {
                            saveGame();
                        }
                        player.setReturnToMenu(true);
                        return -2;
                    }
                    if (selectedOption == 4) { // Thoát game
                        if (confirmconfirmSaveGame()) {
                            saveGame();
                        }
                        SDL_DestroyRenderer(renderer);
                        SDL_DestroyWindow(SDL_GetWindowFromID(1));
                        SDL_Quit();
                        exit(0);
                    }
                    break;
                case SDLK_ESCAPE:
                    return 0;
                }
            }
        }
        renderMenu();
        SDL_Delay(16);
    }
    return 0;
}

void PauseMenu::showConfirmationScreen(const std::string& message) {
    SDL_Event e;
    bool done = false;

    while (!done) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Tạo surface từ text
        SDL_Surface* surface = TTF_RenderUTF8_Blended(font, message.c_str(), { 255, 255, 255, 255 });
        SDL_Texture* msgTexture = SDL_CreateTextureFromSurface(renderer, surface);

        // Lấy kích thước chữ để căn giữa
        int w = surface->w;
        int h = surface->h;
        SDL_Rect msgRect = { (800 - w) / 2, 250, w, h }; // Căn giữa theo chiều ngang

        SDL_FreeSurface(surface); // Giải phóng surface ngay sau khi tạo texture

        SDL_RenderCopy(renderer, msgTexture, NULL, &msgRect);
        SDL_DestroyTexture(msgTexture);

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN || e.type == SDL_QUIT) {
                done = true;
                break;
            }
        }
        SDL_Delay(16);
    }
}

void PauseMenu::saveGame() {
    std::string mazeFile = Menu::chosenSaveFile;  // Truy cập biến static của Menu
    std::string playerFile = mazeFile;

    size_t pos = playerFile.find("_maze.txt");
    if (pos != std::string::npos) 
        playerFile.replace(pos, 9, "_player.txt");
    else 
        playerFile = "Save/default_player.txt"; 

    maze.saveMaze(mazeFile);
    player.savePosition(playerFile);

    showConfirmationScreen("Game đã được lưu!");
}





