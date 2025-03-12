#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "Utils.h"
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
#include "Maze.h"

SDL_Texture* renderText(const std::string &message, TTF_Font *font, SDL_Color color, SDL_Renderer *renderer) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, message.c_str(), color);
    if (!surface) return nullptr;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    SDL_Window* window = SDL_CreateWindow("Shadow Maze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("src/fonts/arial.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return 1;
    }
    
    Menu menu(renderer);
    int gameMode = menu.run();
    if (gameMode == -1) return 0;  // Thoát game

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

        isNewGame = (gameMode == 10);  // Nếu là New Game, sinh mê cung mới
    } else {
        isNewGame = menu.selectGameMode();
        int saveSlot = menu.selectSaveSlot();
        mazeFile = "Save/Save" + std::to_string(saveSlot) + "_maze.txt";
        playerFile = "Save/Save" + std::to_string(saveSlot) + "_player.txt";
    }

    Maze maze;
    std::ifstream mazeCheck(mazeFile);
    if (isNewGame || mazeCheck.fail()) {
        maze.generate();
        maze.saveMaze(mazeFile);
    } else {
        maze.loadMaze(mazeFile);
    }
    mazeCheck.close();

    Player player(maze.getStartX(), maze.getStartY(), renderer);
    std::ifstream playerCheck(playerFile);
    if (!isNewGame && !playerCheck.fail()) {
        player.loadPosition(playerFile);
    }
    playerCheck.close();

    player.loadKeybinds();

    bool running = true;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                PauseMenu pauseMenu(renderer);
                int pauseChoice = pauseMenu.run();
                if (pauseChoice == 1) 
                    player.resetPosition(maze.getStartX(), maze.getStartY());
                if (pauseChoice == -2)
                    running = false;
            }
        }
        
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        player.handleInput(keys, maze);
        player.update(maze);
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        maze.render(renderer);
        player.render(renderer);
        SDL_RenderPresent(renderer);
    }
    
    maze.saveMaze(mazeFile);
    player.savePosition(playerFile);
    
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
