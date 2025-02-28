#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include "Player.h"
#include "Menu.h"
#include "PauseMenu.h"
#include <iostream>
#include <sstream>
#include "SettingsMenu.h"
#include "Maze.h"

SDL_Texture* renderText(const std::string &message, TTF_Font *font, SDL_Color color, SDL_Renderer *renderer) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, message.c_str(), color);
    if (!surface) {
        std::cerr << "Error creating text surface: " << TTF_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cerr << "Error creating texture from surface: " << SDL_GetError() << std::endl;
    }

    return texture;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Shadow Maze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Load font
    TTF_Font* font = TTF_OpenFont("src/fonts/arial.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return 1;
    }

    // Hiển thị menu trước khi vào game
    Menu menu(renderer);
    int choice = menu.run();
    menu.stopMusic();

    if (choice == -1 || choice == 3) {
        std::cout << "Game exited.\n";
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    // 🔹 Khởi tạo mê cung
    Maze maze;
    maze.generate(); // Tạo mê cung ngẫu nhiên

    // 🔹 Đặt nhân vật vào vị trí xuất phát trong mê cung
    int playerStartX = maze.getStartX(); 
    int playerStartY = maze.getStartY();
    Player player(playerStartX, playerStartY, renderer);
    player.loadKeybinds();

    bool running = true;
    SDL_Event e;

    // Biến tính FPS
    Uint32 startTime = SDL_GetTicks();
    int frameCount = 0;
    float fps = 0.0f;
    SDL_Color textColor = {255, 255, 255, 255};

    while (running) {
        Uint32 frameStart = SDL_GetTicks();

        // Xử lý sự kiện
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = false;

            // Xử lý Pause Menu khi nhấn ESC
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                PauseMenu pauseMenu(renderer);
                int pauseChoice = pauseMenu.run();

                if (pauseChoice == 1) { // Chơi lại
                    player.resetPosition(playerStartX, playerStartY);
                } else if (pauseChoice == -2) { // Quay lại menu chính
                    running = false;
                }

                if (!running) { 
                    return main(argc, argv);
                }
            }
        }

        // Nhận input từ bàn phím
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        player.handleInput(keys, maze); // 🔹 Cập nhật để kiểm tra va chạm

        // Cập nhật game
        player.update(maze); 

        // Xóa màn hình
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // 🔹 Vẽ mê cung
        maze.render(renderer);

        // 🔹 Vẽ nhân vật
        player.render(renderer);

        // Tính FPS trung bình mỗi giây
        frameCount++;
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - startTime >= 1000) { 
            fps = frameCount / ((currentTime - startTime) / 1000.0f);
            frameCount = 0;
            startTime = currentTime;
        }

        // Hiển thị FPS
        std::stringstream fpsText;
        fpsText << "FPS: " << (int)fps;
        SDL_Texture* fpsTexture = renderText(fpsText.str(), font, textColor, renderer);

        if (fpsTexture) {
            SDL_Rect fpsRect = {10, 10, 100, 30};
            SDL_RenderCopy(renderer, fpsTexture, NULL, &fpsRect);
            SDL_DestroyTexture(fpsTexture);
        }

        SDL_RenderPresent(renderer);

        // Giữ FPS ổn định
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < 16) {
            SDL_Delay(16 - frameTime);
        }
    }

    // Dọn dẹp
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
