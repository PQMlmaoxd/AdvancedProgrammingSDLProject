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

SDL_Texture* renderText(const std::string &message, TTF_Font *font, SDL_Color color, SDL_Renderer *renderer) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, message.c_str(), color);
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

    // Gọi spawnKey để đặt vị trí key ngẫu nhiên và load texture
    maze.spawnKey(renderer);

    // Tạo Player
    Player player(maze.getStartX(), maze.getStartY(), renderer);
    player.setReturnToMenu(false); // Đặt lại trạng thái trước khi vào game

    // Nếu không phải game mới và tồn tại file player
    std::ifstream playerCheck(playerFile);
    if (!isNewGame && !playerCheck.fail()) {
        player.loadPosition(playerFile);
    }
    playerCheck.close();

    // Tải keybind (các phím di chuyển)
    player.loadKeybinds();

    bool running = true;
    SDL_Event e;
    while (running) {
        // Lấy sự kiện
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return -1;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                // Gọi PauseMenu
                PauseMenu pauseMenu(renderer, maze, player);
                int pauseChoice = pauseMenu.run();
                if (pauseChoice == 1) {
                    // Chơi lại => reset vị trí Player
                    player.resetPosition(maze.getStartX(), maze.getStartY());
                }
                if (pauseChoice == -2) {
                    // Trở về menu chính
                    return 1;
                }
            }
        }

        // Xử lý input của player
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        player.handleInput(keys, maze);

        // Gọi update, trong đó có logic kiểm tra va chạm key và exit
        player.update(maze, renderer);

        // Nếu Player đánh dấu quay lại menu (ví dụ sau khi Win)
        if (player.shouldReturnToMenu()) {
            return 1;
        }

        // Vẽ game
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        maze.render(renderer, player.getX(), player.getY());
        player.render(renderer);
        SDL_RenderPresent(renderer);
    }

    // Khi thoát vòng lặp game, lưu Maze và vị trí Player
    maze.saveMaze(mazeFile);
    player.savePosition(playerFile);
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
        if (gameMode == -1) break; // Thoát game

        createSaveDirectory();

        std::string mazeFile, playerFile;
        bool isNewGame = false;

        std::string chosenFile = menu.getChosenSaveFile();
        if (!chosenFile.empty()) {
            mazeFile = chosenFile;
            playerFile = chosenFile;
            size_t pos = playerFile.find("_maze.txt");
            if (pos != std::string::npos)
                playerFile.replace(pos, 9, "_player.txt");
            else
                playerFile = "Save/default_player.txt";
            isNewGame = (gameMode == 10);
        } else {
            isNewGame = menu.selectGameMode();
            int saveSlot = menu.selectSaveSlot();
            mazeFile = "Save/Save" + std::to_string(saveSlot) + "_maze.txt";
            playerFile = "Save/Save" + std::to_string(saveSlot) + "_player.txt";
        }

        int gameResult = startGame(renderer, mazeFile, playerFile, isNewGame);
        Menu::chosenSaveFile = "";
        if (gameResult == -1) break; // Nếu thoát game
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
