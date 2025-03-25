#include "Menu.h"
#include "Maze.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <SDL_image.h>
#include <filesystem> 



// Định nghĩa biến tĩnh
std::string Menu::chosenSaveFile = "";

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

Menu::Menu(SDL_Renderer* renderer) 
    : renderer(renderer), selectedOption(0), firstPlay(true), blinkTimer(0), blinkState(true) {
    font = TTF_OpenFont("resources/fonts/arial-unicode-ms.ttf", 28);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    }

    // Khởi tạo SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! Error: " << Mix_GetError() << std::endl;
    }
    loadSettings(); // Load settings ngay khi game khởi động
    backgroundMusic = Mix_LoadMUS("resources/audio/menu_music.mp3");
    if (!backgroundMusic) {
        std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
    }

    textColor = {255, 255, 255, 255};
    options = {"Singleplayer", "2 Players", "Guide", "Settings", "Thoát"};

    SDL_Surface* bgSurface = IMG_Load("resources/images/menu_background.jpg");
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

	std::filesystem::path p{"Save"};
    if (!std::filesystem::exists(p)) return ""; // Nếu thư mục Save không tồn tại

	for (auto& entry : std::filesystem::directory_iterator(p)) {
		std::string filename = entry.path().filename().string();
		if (filename.find(".txt") != std::string::npos) {
			saveFiles.push_back(filename);
		}
	}
	
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

void Menu::switchToGameMusic() {
    // Dừng nhạc menu
    Mix_HaltMusic();
    // Giải phóng bản nhạc menu nếu cần (nếu bạn không còn dùng nữa)
    Mix_FreeMusic(backgroundMusic);
    // Tải nhạc game mới
    Mix_Music* gameMusic = Mix_LoadMUS("resources/audio/game_music.mp3");
    if (!gameMusic) {
        std::cerr << "Failed to load game music: " << Mix_GetError() << std::endl;
        return;
    }
    // Phát nhạc game, lặp vô hạn
    Mix_PlayMusic(gameMusic, -1);
    // Nếu bạn muốn lưu lại gameMusic để giải phóng sau này, hãy gán lại cho biến backgroundMusic hoặc biến riêng.
    backgroundMusic = gameMusic;
}

int Menu::run() {
    bool running = true;
    SDL_Event e;
    playMusic();

    // Maze loadedMaze; 
    // (Nếu bạn không dùng loadedMaze trong Singleplayer thì có thể xóa dòng này)

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return -1;

            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_UP) {
                    selectedOption = (selectedOption - 1 + options.size()) % options.size();
                }
                else if (e.key.keysym.sym == SDLK_DOWN) {
                    selectedOption = (selectedOption + 1) % options.size();
                }
                else if (e.key.keysym.sym == SDLK_RETURN) {
                    // ======= SINGLEPLAYER =======
                    if (selectedOption == 0) {
                        // Hỏi New Game / Load Game
                        int choice = chooseNewOrLoad();
                        if (choice == 0) {  // New Game
                            int ret = handleNewGame();
                            if (ret != -1) {
                                switchToGameMusic();
                                // Giá trị trả về để main biết vào Singleplayer
                                // ret có thể là 10 (bạn định nghĩa ở handleNewGame)
                                return ret;
                            }
                        }
                        else if (choice == 1) {  // Load Game
                            int ret = handleLoadGame();
                            if (ret == 20) {
                                switchToGameMusic();
                                // 20: Vào game Singleplayer ngay
                                return 20;
                            }
                        }
                    }
                    // ======= 2 PLAYERS =======
                    else if (selectedOption == 1) {
                        // Bỏ qua chooseNewOrLoad(), luôn tạo map mới
                        switchToGameMusic();
                        // Trả về 2 (hoặc 30, 40...) để main biết là 2 Player
                        return 2;
                    }
                    // ======= GUIDE =======
                    else if (selectedOption == 2) {
                        showGuide();
                    }
                    // ======= SETTINGS =======
                    else if (selectedOption == 3) {
                        SettingsMenu settings(renderer);
                        settings.run();
                        loadSettings();
                    }
                    // ======= THOÁT =======
                    else if (selectedOption == 4) {
                        if (confirmExit()) return -1;
                    }
                }
            }
        }

        // Nếu nhạc bị tắt, phát lại
        if (!Mix_PlayingMusic()) {
            playMusic();
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

    // Mở font với cỡ 28 (đảm bảo file font tồn tại)
    TTF_Font* font = TTF_OpenFont("resources/fonts/arial-unicode-ms.ttf", 28);
    if (!font) {
        std::cerr << "Failed to load font in selectGameMode: " << TTF_GetError() << std::endl;
        return false;
    }
    SDL_Color textColor = { 255, 255, 255, 255 };

    while (choosing) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                TTF_CloseFont(font);
                return false;
            }
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_LEFT:
                case SDLK_RIGHT:
                    selected = 1 - selected;
                    break;
                case SDLK_RETURN:
                    TTF_CloseFont(font);
                    return (selected == 0);
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Xây dựng chuỗi hiển thị cho các lựa chọn
        std::string newGameStr = (selected == 0 ? "> New Game <" : "New Game");
        std::string loadGameStr = (selected == 1 ? "> Load Game <" : "Load Game");

        // Render New Game
        SDL_Surface* newGameSurface = TTF_RenderUTF8_Blended(font, newGameStr.c_str(), textColor);
        if (!newGameSurface) {
            std::cerr << "Failed to render New Game surface: " << TTF_GetError() << std::endl;
            continue;
        }
        SDL_Texture* newGameText = SDL_CreateTextureFromSurface(renderer, newGameSurface);
        int newGameW, newGameH;
        SDL_QueryTexture(newGameText, NULL, NULL, &newGameW, &newGameH);
        SDL_Rect newGameRect = { 300, 200, newGameW, newGameH };

        // Render Load Game
        SDL_Surface* loadGameSurface = TTF_RenderUTF8_Blended(font, loadGameStr.c_str(), textColor);
        if (!loadGameSurface) {
            std::cerr << "Failed to render Load Game surface: " << TTF_GetError() << std::endl;
            SDL_DestroyTexture(newGameText);
            SDL_FreeSurface(newGameSurface);
            continue;
        }
        SDL_Texture* loadGameText = SDL_CreateTextureFromSurface(renderer, loadGameSurface);
        int loadGameW, loadGameH;
        SDL_QueryTexture(loadGameText, NULL, NULL, &loadGameW, &loadGameH);
        SDL_Rect loadGameRect = { 300, 300, loadGameW, loadGameH };

        // Render cả hai lựa chọn
        SDL_RenderCopy(renderer, newGameText, NULL, &newGameRect);
        SDL_RenderCopy(renderer, loadGameText, NULL, &loadGameRect);

        SDL_DestroyTexture(newGameText);
        SDL_DestroyTexture(loadGameText);
        SDL_FreeSurface(newGameSurface);
        SDL_FreeSurface(loadGameSurface);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    TTF_CloseFont(font);
    return false;
}

int Menu::selectSaveSlot() {
    SDL_Event e;
    int selectedSlot = 1;
    bool choosing = true;

    // Mở font với cỡ 28
    TTF_Font* font = TTF_OpenFont("resources/fonts/arial-unicode-ms.ttf", 28);
    if (!font) {
        std::cerr << "Failed to load font in selectSaveSlot: " << TTF_GetError() << std::endl;
        return 1;
    }
    SDL_Color textColor = { 255, 255, 255, 255 };

    while (choosing) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                TTF_CloseFont(font);
                return 1;
            }
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_LEFT:
                    if (selectedSlot > 1) selectedSlot--;
                    break;
                case SDLK_RIGHT:
                    if (selectedSlot < 3) selectedSlot++; // Giả sử có tối đa 3 slot
                    break;
                case SDLK_RETURN:
                    TTF_CloseFont(font);
                    return selectedSlot;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Tạo thông điệp hiển thị slot
        std::string slotText = "Save Slot: " + std::to_string(selectedSlot);
        SDL_Surface* surface = TTF_RenderUTF8_Blended(font, slotText.c_str(), textColor);
        if (!surface) {
            std::cerr << "Failed to render save slot surface: " << TTF_GetError() << std::endl;
            continue;
        }
        SDL_Texture* slotTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!slotTexture) {
            std::cerr << "Failed to create save slot texture: " << SDL_GetError() << std::endl;
            continue;
        }
        int textW, textH;
        SDL_QueryTexture(slotTexture, NULL, NULL, &textW, &textH);
        // Căn giữa theo chiều ngang, vị trí y là 250
        SDL_Rect slotRect = { (800 - textW) / 2, 250, textW, textH };

        SDL_RenderCopy(renderer, slotTexture, NULL, &slotRect);
        SDL_DestroyTexture(slotTexture);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    TTF_CloseFont(font);
    return 1;
}

void Menu::renderSubMenu(const std::vector<std::string>& options, int selected) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    TTF_Font* font = TTF_OpenFont("resources/fonts/arial-unicode-ms.ttf", 28);
    if (!font) {
        std::cerr << "Failed to load font in renderSubMenu: " << TTF_GetError() << std::endl;
        return;
    }
    SDL_Color textColor = { 255, 255, 255, 255 };

    for (size_t i = 0; i < options.size(); i++) {
        std::string displayText = (i == selected) ? ("> " + options[i] + " <") : options[i];
        SDL_Surface* surface = TTF_RenderUTF8_Blended(font, displayText.c_str(), textColor);
        if (!surface) {
            std::cerr << "Failed to render text surface: " << TTF_GetError() << std::endl;
            continue;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            std::cerr << "Failed to create text texture: " << SDL_GetError() << std::endl;
            SDL_FreeSurface(surface);
            continue;
        }
        // Tùy chỉnh vị trí và kích thước dựa trên kích thước của surface
        SDL_Rect rect = { 300, 200 + static_cast<int>(i) * 50, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }

    TTF_CloseFont(font);
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
    if (!Mix_PlayingMusic()) {
        Mix_PlayMusic(backgroundMusic, -1);
    }
}

void Menu::showGuide() {
    std::ifstream file("resources/data/guide.txt");
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
            SDL_Surface* surface = TTF_RenderUTF8_Blended(font, line.c_str(), { 255, 255, 255, 255 });
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, surface);
            if (textTexture) {
                SDL_Rect textRect = {100, yOffset, surface->w, surface->h};
                SDL_FreeSurface(surface);
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
        Mix_HaltMusic();
    }
}

bool Menu::confirmExit() {
    SDL_Event e;
    bool choosing = true;
    int selected = 0; // 0 = Không, 1 = Đúng

    // Giả sử font đã được load trước đó (hoặc bạn có thể load lại tại đây)
    TTF_Font* font = TTF_OpenFont("resources/fonts/arial-unicode-ms.ttf", 28);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return false;
    }

    while (choosing) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                TTF_CloseFont(font);
                return true;
            }
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_LEFT:
                case SDLK_RIGHT:
                    selected = 1 - selected;
                    break;
                case SDLK_RETURN:
                    TTF_CloseFont(font);
                    return selected == 1;
                case SDLK_ESCAPE:
                    TTF_CloseFont(font);
                    return false;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render thông điệp hỏi người dùng
        SDL_Surface* messageSurface = TTF_RenderUTF8_Blended(font, "Bạn thực sự muốn thoát game?", { 255, 255, 255, 255 });
        SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(renderer, messageSurface);
        SDL_Rect messageRect = { 200, 200, messageSurface->w, messageSurface->h };
        SDL_RenderCopy(renderer, messageTexture, NULL, &messageRect);
        SDL_DestroyTexture(messageTexture);
        SDL_FreeSurface(messageSurface);

        // Render lựa chọn "Có"
        std::string yesStr = (selected == 1 ? "> Đúng <" : "Đúng");
        SDL_Surface* yesSurface = TTF_RenderUTF8_Blended(font, yesStr.c_str(), { 255, 255, 255, 255 });
        SDL_Texture* yesTexture = SDL_CreateTextureFromSurface(renderer, yesSurface);
        SDL_Rect yesRect = { 250, 300, yesSurface->w, yesSurface->h };
        SDL_RenderCopy(renderer, yesTexture, NULL, &yesRect);
        SDL_DestroyTexture(yesTexture);
        SDL_FreeSurface(yesSurface);

        // Render lựa chọn "Không"
        std::string noStr = (selected == 0 ? "> Không <" : "Không");
        SDL_Surface* noSurface = TTF_RenderUTF8_Blended(font, noStr.c_str(), { 255, 255, 255, 255 });
        SDL_Texture* noTexture = SDL_CreateTextureFromSurface(renderer, noSurface);
        SDL_Rect noRect = { 450, 300, noSurface->w, noSurface->h };
        SDL_RenderCopy(renderer, noTexture, NULL, &noRect);
        SDL_DestroyTexture(noTexture);
        SDL_FreeSurface(noSurface);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    return false;
}

SDL_Texture* Menu::renderText(const std::string& text, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void Menu::renderMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Vẽ ảnh nền nếu có
    if (backgroundTexture) {
        SDL_Rect destRect = { 0, 0, 800, 600 };
        SDL_RenderCopy(renderer, backgroundTexture, NULL, &destRect);
    }

    // Cập nhật hiệu ứng nhấp nháy
    blinkTimer++;
    if (blinkTimer >= 30) {
        blinkTimer = 0;
        blinkState = !blinkState;
    }

    // Mở font (bạn có thể thay đổi font nếu cần)
    TTF_Font* font = TTF_OpenFont("resources/fonts/arial-unicode-ms.ttf", 28);
    if (!font) {
        std::cerr << "Failed to load font in renderMenu: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Rect highlightRect = { 0, 0, 0, 0 }; // Khởi tạo highlightRect

    for (size_t i = 0; i < options.size(); i++) {
        // Sử dụng TTF_RenderUTF8_Blended để tạo surface với hiệu ứng mượt mà
        std::string optionStr = options[i];
        SDL_Surface* surface = TTF_RenderUTF8_Blended(font, optionStr.c_str(), textColor);
        if (!surface) {
            std::cerr << "Failed to render text surface: " << TTF_GetError() << std::endl;
            continue;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            std::cerr << "Failed to create text texture: " << SDL_GetError() << std::endl;
            SDL_FreeSurface(surface);
            continue;
        }
        int w, h;
        SDL_QueryTexture(texture, NULL, NULL, &w, &h);
        SDL_Rect rect = { (800 - w) / 2, 200 + static_cast<int>(i) * 60, w, h };
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        if (i == selectedOption) {
            highlightRect = { rect.x - 10, rect.y - 5, rect.w + 20, rect.h + 10 };
        }
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }

    if (blinkState) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderDrawRect(renderer, &highlightRect);
    }

    TTF_CloseFont(font);
    SDL_RenderPresent(renderer);
}

std::string Menu::getChosenSaveFile() {
    return chosenSaveFile;
}

void Menu::showConfirmationScreen(const std::string& message) {
    SDL_Event e;
    bool done = false;

    TTF_Font* font = TTF_OpenFont("resources/fonts/arial-unicode-ms.ttf", 28);
    if (!font) {
        std::cerr << "Failed to load font in showConfirmationScreen: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Color color = { 255, 255, 255, 255 };

    while (!done) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Tạo surface bằng TTF_RenderUTF8_Blended
        SDL_Surface* surface = TTF_RenderUTF8_Blended(font, message.c_str(), color);
        if (!surface) {
            std::cerr << "Failed to render text surface: " << TTF_GetError() << std::endl;
        }
        else {
            SDL_Texture* msgTexture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
            if (msgTexture) {
                // Sử dụng kích thước từ texture; ở đây ta dùng kích thước cố định nếu cần
                SDL_Rect msgRect = { 50, 250, 700, 50 };
                SDL_RenderCopy(renderer, msgTexture, NULL, &msgRect);
                SDL_DestroyTexture(msgTexture);
            }
        }

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN || e.type == SDL_QUIT) {
                done = true;
                break;
            }
        }
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
}

int Menu::handleNewGame() {
    // 1) Gọi promptForSaveName
    std::string saveFileName = promptForSaveName(renderer, font);
    if (!saveFileName.empty()) {
        // 2) Hỏi xác nhận
        bool confirm = confirmSaveFile(saveFileName);
        if (!confirm) {
            // Người chơi chọn Không => về lại menu
            return -1; 
        }
        // Nếu confirm = true => tạo file
        std::string fullPath = "Save/" + saveFileName + ".txt";
        Maze newMaze(true);
        newMaze.saveMaze(fullPath);
        chosenSaveFile = fullPath;
        showConfirmationScreen("File save \"" + saveFileName + ".txt\" da duoc tao thanh cong!");
        return 10; 
    }
    return -1;
}

bool Menu::confirmSaveFile(const std::string& fileName) {
    SDL_Event e;
    bool choosing = true;
    int selected = 0; // 0 = Không, 1 = Có

    // Mở font để render text (cỡ 28)
    TTF_Font* font = TTF_OpenFont("resources/fonts/arial-unicode-ms.ttf", 28);
    if (!font) {
        std::cerr << "Failed to load font in confirmSaveFile: " << TTF_GetError() << std::endl;
        return false;
    }

    SDL_Color textColor = { 255, 255, 255, 255 };

    while (choosing) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                TTF_CloseFont(font);
                return false;
            }
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_LEFT:
                case SDLK_RIGHT:
                case SDLK_UP:
                case SDLK_DOWN:
                    selected = 1 - selected; // Chuyển giữa Có (1) và Không (0)
                    break;
                case SDLK_RETURN:
                    TTF_CloseFont(font);
                    return (selected == 1); // Nếu =1 => Có, ngược lại false
                case SDLK_ESCAPE:
                    TTF_CloseFont(font);
                    return false;
                }
            }
        }

        // Vẽ hộp thoại xác nhận
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Hiển thị thông điệp: "FileSave: <fileName> \n Co xac nhan khong?"
        std::string message = "FileSave: " + fileName + "\nCo xac nhan khong?";
        SDL_Surface* msgSurface = TTF_RenderUTF8_Blended(font, message.c_str(), textColor);
        if (!msgSurface) {
            std::cerr << "Failed to render message surface: " << TTF_GetError() << std::endl;
        }
        else {
            SDL_Texture* msgTexture = SDL_CreateTextureFromSurface(renderer, msgSurface);
            SDL_FreeSurface(msgSurface);
            if (msgTexture) {
                int w, h;
                SDL_QueryTexture(msgTexture, NULL, NULL, &w, &h);
                SDL_Rect msgRect = { (800 - w) / 2, 200, w, h };
                SDL_RenderCopy(renderer, msgTexture, NULL, &msgRect);
                SDL_DestroyTexture(msgTexture);
            }
        }

        // Hiển thị lựa chọn "Có" và "Không"
        std::string yesOption = (selected == 1) ? "> Co <" : "Co";
        std::string noOption = (selected == 0) ? "> Khong <" : "Khong";

        // Render "Có"
        SDL_Surface* yesSurface = TTF_RenderUTF8_Blended(font, yesOption.c_str(), textColor);
        if (yesSurface) {
            SDL_Texture* yesTexture = SDL_CreateTextureFromSurface(renderer, yesSurface);
            int w, h;
            SDL_QueryTexture(yesTexture, NULL, NULL, &w, &h);
            SDL_Rect yesRect = { 200, 300, w, h };
            SDL_RenderCopy(renderer, yesTexture, NULL, &yesRect);
            SDL_DestroyTexture(yesTexture);
            SDL_FreeSurface(yesSurface);
        }

        // Render "Không"
        SDL_Surface* noSurface = TTF_RenderUTF8_Blended(font, noOption.c_str(), textColor);
        if (noSurface) {
            SDL_Texture* noTexture = SDL_CreateTextureFromSurface(renderer, noSurface);
            int w, h;
            SDL_QueryTexture(noTexture, NULL, NULL, &w, &h);
            SDL_Rect noRect = { 400, 300, w, h };
            SDL_RenderCopy(renderer, noTexture, NULL, &noRect);
            SDL_DestroyTexture(noTexture);
            SDL_FreeSurface(noSurface);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    return false;
}

int Menu::handleLoadGame() {
    std::string saveFileName = chooseSaveFile();
    if (!saveFileName.empty()) {
        chosenSaveFile = "Save/" + saveFileName;
        showConfirmationScreen("Load thanh cong! Vao game ngay lap tuc!");
        return 20;  // Trả về giá trị để vào game ngay
    }
    return -1;
}
