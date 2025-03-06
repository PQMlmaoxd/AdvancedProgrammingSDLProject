#include "Menu.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <SDL2/SDL_image.h>
#include <dirent.h>  // Thư viện duyệt thư mục
#ifdef _WIN32
    #include <direct.h>  // Windows: _mkdir
#else
    #include <sys/stat.h>  // Linux/macOS: mkdir
    #include <sys/types.h>
#endif


void Menu::loadSettings() {
    std::ifstream file("settings.txt");
    if (file.is_open()) {
        std::string key;
        int value;
        while (file >> key >> value) {
            if (key == "volume") {
                Mix_VolumeMusic(value * MIX_MAX_VOLUME / 100);
            }
        }
        file.close();
    }
}

Menu::Menu(SDL_Renderer* renderer) : renderer(renderer), selectedOption(0), firstPlay(true), blinkTimer(0), blinkState(true) {
    font = TTF_OpenFont("src/fonts/arial-unicode-ms.ttf", 28);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    }

    // Khởi tạo SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! Error: " << Mix_GetError() << std::endl;
    }
    loadSettings(); // Load settings ngay khi game khởi động
    backgroundMusic = Mix_LoadMUS("src/audio/menu_music.mp3");
    if (!backgroundMusic) {
        std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
    }

    textColor = {255, 255, 255, 255};
    options = {"Singleplayer", "2 Players", "Guide", "Settings", "Thoát"};

    SDL_Surface* bgSurface = IMG_Load("src/images/menu_background.jpg");
    if (!bgSurface) {
        std::cerr << "Failed to load background image: " << IMG_GetError() << std::endl;
    } else {
        backgroundTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
        SDL_FreeSurface(bgSurface);
    }
}

int Menu::chooseNewOrLoad() {
    std::vector<std::string> choices = {"New Game", "Load Game"};
    int selection = 0;
    SDL_Event e;
    while (true) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return -1;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_UP) selection = (selection - 1 + 2) % 2;
                if (e.key.keysym.sym == SDLK_DOWN) selection = (selection + 1) % 2;
                if (e.key.keysym.sym == SDLK_RETURN) return selection;
                if (e.key.keysym.sym == SDLK_ESCAPE) return -1;
            }
        }
        renderSubMenu(choices, selection);
    }
}

std::string Menu::chooseSaveFile() {
    std::vector<std::string> saveFiles;

    DIR* dir = opendir("Save");
    if (!dir) return ""; // Nếu thư mục Save không tồn tại

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string filename = entry->d_name;
        if (filename.find(".txt") != std::string::npos) {
            saveFiles.push_back(filename);
        }
    }
    closedir(dir);

    if (saveFiles.empty()) return "";
    int selection = 0;
    SDL_Event e;
    while (true) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return "";
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_UP) selection = (selection - 1 + saveFiles.size()) % saveFiles.size();
                if (e.key.keysym.sym == SDLK_DOWN) selection = (selection + 1) % saveFiles.size();
                if (e.key.keysym.sym == SDLK_RETURN) return saveFiles[selection];
                if (e.key.keysym.sym == SDLK_ESCAPE) return "";
            }
        }
        renderSubMenu(saveFiles, selection);
    }
}


int Menu::run() {
    bool running = true;
    SDL_Event e;
    playMusic();

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return -1;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_UP) selectedOption = (selectedOption - 1 + options.size()) % options.size();
                if (e.key.keysym.sym == SDLK_DOWN) selectedOption = (selectedOption + 1) % options.size();
                if (e.key.keysym.sym == SDLK_RETURN) {
                    if (selectedOption == 0 || selectedOption == 1) {
                        int gameMode = selectedOption; 
                        int choice = chooseNewOrLoad();
                        if (choice == 0) return gameMode + 10; 
                        if (choice == 1) {
                            std::string saveFile = chooseSaveFile();
                            if (!saveFile.empty()) return gameMode + 20;
                        }
                    } else if (selectedOption == 2) {
                        showGuide();
                    } else if (selectedOption == 3) {
                        SettingsMenu settings(renderer);
                        settings.run();
                        loadSettings();
                    } else if (selectedOption == 4) {
                        if (confirmExit()) return -1;
                    }
                }
            }
        }
        renderMenu();
        SDL_Delay(16);
    }
    return -1;
}

bool Menu::selectGameMode() {
    SDL_Event e;
    int selected = 0; // 0 = New Game, 1 = Load Game
    bool choosing = true;

    while (choosing) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return false;
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                    case SDLK_RIGHT:
                        selected = 1 - selected; // Chuyển giữa New Game và Load Game
                        break;
                    case SDLK_RETURN:
                        return selected == 0; // True nếu chọn New Game
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Texture* newGameText = renderText(selected == 0 ? "> New Game <" : "New Game", {255, 255, 255, 255});
        SDL_Texture* loadGameText = renderText(selected == 1 ? "> Load Game <" : "Load Game", {255, 255, 255, 255});

        SDL_Rect newGameRect = {300, 200, 200, 40};
        SDL_Rect loadGameRect = {300, 300, 200, 40};

        SDL_RenderCopy(renderer, newGameText, NULL, &newGameRect);
        SDL_RenderCopy(renderer, loadGameText, NULL, &loadGameRect);

        SDL_DestroyTexture(newGameText);
        SDL_DestroyTexture(loadGameText);

        SDL_RenderPresent(renderer);
    }
    return false;
}

int Menu::selectSaveSlot() {
    SDL_Event e;
    int selectedSlot = 1;
    bool choosing = true;

    while (choosing) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return 1;
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        if (selectedSlot > 1) selectedSlot--;
                        break;
                    case SDLK_RIGHT:
                        if (selectedSlot < 3) selectedSlot++; // Giả sử có tối đa 3 slot
                        break;
                    case SDLK_RETURN:
                        return selectedSlot;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        std::string slotText = "Save Slot: " + std::to_string(selectedSlot);
        SDL_Texture* slotTexture = renderText(slotText, {255, 255, 255, 255});
        SDL_Rect slotRect = {300, 250, 200, 40};

        SDL_RenderCopy(renderer, slotTexture, NULL, &slotRect);
        SDL_DestroyTexture(slotTexture);

        SDL_RenderPresent(renderer);
    }
    return 1;
}

void Menu::renderSubMenu(const std::vector<std::string>& options, int selected) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (size_t i = 0; i < options.size(); i++) {
        SDL_Texture* texture = renderText(i == selected ? "> " + options[i] + " <" : options[i], {255, 255, 255, 255});
        SDL_Rect rect = {300, 200 + (int)i * 50, 200, 40};
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(renderer);
}

Menu::~Menu() {
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
    }
    Mix_FreeMusic(backgroundMusic);
    Mix_CloseAudio();
    TTF_CloseFont(font);
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
    SDL_Event e;
    bool viewingGuide = true;
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
            SDL_Texture* textTexture = renderText(line, {255, 255, 255, 255});
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

bool Menu::confirmExit() {
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
                    return selected == 1; // Trả về true nếu chọn "Đúng"
                    case SDLK_ESCAPE:
                        return false; // Hủy thoát
                    }
                }
            }
    
            // Hiển thị hộp thoại xác nhận
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
    
            SDL_Texture* message = renderText("Bạn thực sự muốn thoát game?", {255, 255, 255, 255});
            SDL_Rect messageRect = {200, 200, 400, 50};
            SDL_RenderCopy(renderer, message, NULL, &messageRect);
            SDL_DestroyTexture(message);
    
            SDL_Texture* yesText = renderText(selected == 1 ? "> Đúng <" : "Đúng", {255, 255, 255, 255});
            SDL_Texture* noText = renderText(selected == 0 ? "> Không <" : "Không", {255, 255, 255, 255});
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

SDL_Texture* Menu::renderText(const std::string& text, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderUTF8_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void Menu::renderMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Vẽ ảnh nền nếu có
    if (backgroundTexture) {
        SDL_Rect destRect = {0, 0, 800, 600};  // Full màn hình 800x600
        SDL_RenderCopy(renderer, backgroundTexture, NULL, &destRect);
    }

    // Cập nhật hiệu ứng nhấp nháy
    blinkTimer++;
    if (blinkTimer >= 30) {  // Mỗi 30 frame đổi trạng thái nhấp nháy
        blinkTimer = 0;
        blinkState = !blinkState;
    }

    for (size_t i = 0; i < options.size(); i++) {
        // Mục được chọn sẽ nhấp nháy
        if (i == selectedOption && !blinkState) continue;

        // Đổi màu nếu đang chọn
        SDL_Color color = (i == selectedOption) ? SDL_Color{255, 255, 0, 255}  // Màu vàng khi chọn
                                                 : SDL_Color{255, 255, 255, 255}; // Màu trắng mặc định
        
        SDL_Texture* texture = renderText(options[i], {255, 255, 255, 255});
        int w, h;
        SDL_QueryTexture(texture, NULL, NULL, &w, &h);

        // Căn giữa theo chiều ngang
        SDL_Rect rect = { (800 - w) / 2, 200 + (int)i * 60, w, h };
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(renderer);
}
