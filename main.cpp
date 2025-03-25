#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <sstream>
#include <fstream>

#ifdef _WIN32
    #include <direct.h>
#else
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

#include "Player.h"
#include "Menu.h"
#include "PauseMenu.h"
#include "SettingsMenu.h"
#include "Player.h"
#include "Utils.h"
#include "TwoPlayerMode.h"

SDL_Texture* renderText(const std::string &message, TTF_Font *font, SDL_Color color, SDL_Renderer *renderer) {
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, message.c_str(), color);
    if (!surface) return nullptr;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

int startGame(SDL_Renderer* renderer, const std::string& mazeFile, const std::string& playerFile, bool isNewGame) {
    Maze maze;
    std::ifstream mazeCheck(mazeFile);
    if (isNewGame || mazeCheck.fail()) {
        maze.generate();
        maze.saveMaze(mazeFile);
    }
    else {
        maze.loadMaze(mazeFile);
    }
    mazeCheck.close();

    // Tạo Player
    Player player(maze.getStartX(), maze.getStartY(), renderer);
    player.setReturnToMenu(false);

    std::ifstream playerCheck(playerFile);
    if (!isNewGame && !playerCheck.fail()) {
        player.loadPosition(playerFile);
    }
    playerCheck.close();

    player.loadKeybinds();

    // Khởi tạo timer: lưu thời gian bắt đầu game (tính theo milli giây)
    Uint32 startTime = SDL_GetTicks();
    player.setStartTime(startTime);

    // Tạo font cho timer
    TTF_Font* timerFont = TTF_OpenFont("resources/fonts/arial.ttf", 24);
    if (!timerFont) {
        std::cerr << "Failed to load timer font: " << TTF_GetError() << std::endl;
    }
    SDL_Color timerColor = { 255, 255, 255, 255 };

    bool running = true;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                return -1;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                PauseMenu pauseMenu(renderer, maze, player);
                int pauseChoice = pauseMenu.run();
                if (pauseChoice == 1) {
                    player.resetPosition(maze.getStartX(), maze.getStartY());
                    startTime = SDL_GetTicks();
                    player.setStartTime(startTime);
                }
                if (pauseChoice == -2) {
                    return 1; // Quay về menu chính
                }
            }
        }

        // Tính thời gian đã trôi qua
        Uint32 elapsedTime = SDL_GetTicks() - startTime;

        // Gọi spawnKey mỗi vòng lặp, key sẽ chỉ xuất hiện sau 20s (và chỉ spawn một lần)
        maze.spawnKey(renderer, elapsedTime);

        // Cập nhật thời gian chơi của player
        player.setPlayTime(elapsedTime);

        const Uint8* keys = SDL_GetKeyboardState(NULL);
        player.handleInput(keys, maze);
        player.update(maze, renderer);

        // Nếu player thắng (được xử lý trong Player::update), thoát vòng lặp
        if (player.shouldReturnToMenu()) {
            return 1;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        maze.render(renderer, player.getX(), player.getY());
        player.render(renderer);

        // Render đồng hồ (timer) ở góc trên bên phải
        {
            std::ostringstream oss;
            oss << "Time: " << elapsedTime << " ms";
            SDL_Texture* timerText = renderText(oss.str(), timerFont, timerColor, renderer);
            if (timerText) {
                int textW, textH;
                SDL_QueryTexture(timerText, NULL, NULL, &textW, &textH);
                SDL_Rect timerRect = { SCREEN_WIDTH - textW - 10, 10, textW, textH };
                SDL_RenderCopy(renderer, timerText, NULL, &timerRect);
                SDL_DestroyTexture(timerText);
            }
        }

        SDL_RenderPresent(renderer);
    }

    maze.saveMaze(mazeFile);
    player.savePosition(playerFile);

    TTF_CloseFont(timerFont);
    return 0;
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    SDL_Window* window = SDL_CreateWindow("Shadow Maze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("resources/fonts/arial.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return 1;
    }

    while (true) {
        Menu menu(renderer);
        int gameMode = menu.run();

        // Nếu người chơi chọn "Thoát" trong Menu => gameMode == -1
        if (gameMode == -1) {
            break; // Thoát hẳn chương trình
        }

        // ================================
        // 1) Kiểm tra 2 PLAYER MODE
        // ================================
        if (gameMode == 2) {
            // 2 người chơi
            TwoPlayerMode twoPlayer(renderer);
            int ret = twoPlayer.run(); // Giả sử twoPlayer.run() trả về -1 nếu người chơi muốn thoát game

            if (ret == -1) {
                // Người chơi thoát trong chế độ 2P => thoát hẳn game
                break;
            }
            // Nếu ret != -1 => quay lại vòng lặp => hiển thị Menu tiếp
            continue;
        }

        // ================================
        // 2) SINGLEPLAYER (mặc định)
        // ================================
        createSaveDirectory();

        std::string mazeFile, playerFile;
        bool isNewGame = false;

        std::string chosenFile = menu.getChosenSaveFile();
        if (!chosenFile.empty()) {
            mazeFile = chosenFile;
            playerFile = chosenFile;
            size_t pos = playerFile.find("_maze.txt");
            if (pos != std::string::npos) {
                playerFile.replace(pos, 9, "_player.txt");
            }
            else {
                playerFile = "Save/default_player.txt";
            }
            isNewGame = (gameMode == 10); // Ví dụ: gameMode == 10 là "New Game"
        }
        else {
            isNewGame = menu.selectGameMode();
            int saveSlot = menu.selectSaveSlot();
            mazeFile = "Save/Save" + std::to_string(saveSlot) + "_maze.txt";
            playerFile = "Save/Save" + std::to_string(saveSlot) + "_player.txt";
        }

        // Gọi hàm chơi game Singleplayer
        int gameResult = startGame(renderer, mazeFile, playerFile, isNewGame);
        Menu::chosenSaveFile = "";

        // Nếu người chơi nhấn ESC (gameResult == -1), thoát hẳn
        if (gameResult == -1) {
            break;
        }
        // Nếu gameResult == 1,... => quay lại menu => while(true) lặp lại
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
    
    return 0;
}
