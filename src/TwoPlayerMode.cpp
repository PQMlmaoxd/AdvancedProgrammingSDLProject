#include "TwoPlayerMode.h"
#include <iostream>
#include <SDL_ttf.h>
#include "Player.h"

// Constructor
TwoPlayerMode::TwoPlayerMode(SDL_Renderer* renderer)
    : renderer(renderer),
    maze(true),
    player1(maze.getStartX(), maze.getStartY(), renderer),
    player2(maze.getGoalX(), maze.getStartY(), renderer)
{
    // Ghi nhận thời điểm bắt đầu 2P mode
    startTime2P = SDL_GetTicks();

    // Nếu player2 spawn trúng tường, dịch sang (40,40)
    SDL_Rect spawnRect = player2.getRect();
    if (maze.checkCollision(spawnRect)) {
        player2.resetPosition(40, 40);
    }

    // Player1: WASD
    player1.setKeybind("left", SDLK_a);
    player1.setKeybind("right", SDLK_d);
    player1.setKeybind("up", SDLK_w);
    player1.setKeybind("down", SDLK_s);

    // Player2: arrow keys
    player2.setKeybind("left", SDLK_LEFT);
    player2.setKeybind("right", SDLK_RIGHT);
    player2.setKeybind("up", SDLK_UP);
    player2.setKeybind("down", SDLK_DOWN);

    running = true;
}

TwoPlayerMode::~TwoPlayerMode() {
    // Hủy tài nguyên nếu cần
}

int TwoPlayerMode::run() {
    while (running) {
        handleEvents();
        update();
        render();
        SDL_Delay(16); // ~60 FPS
    }
    return returnValue; // Quay lại menu
}

void TwoPlayerMode::handleEvents() {
    SDL_Event e;
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        }
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        }
    }

    // Xử lý di chuyển
    player1.handleInput(keys, maze);
    player2.handleInput(keys, maze);
}

void TwoPlayerMode::update() {
    // Tính thời gian trôi qua kể từ khi bắt đầu 2P
    Uint32 elapsedTime = SDL_GetTicks() - startTime2P;

    // Gọi hàm spawnKey => key xuất hiện sau 20s
    maze.spawnKey(renderer, elapsedTime);

    // Player 1 chỉ di chuyển, không ăn key, không mở cửa
    player1.updateNoKey(maze, renderer);

    // Player 2 cập nhật đầy đủ
    player2.update1(maze, renderer);

    // Kiểm tra thắng (chỉ Player 2)
    checkWinCondition();
}

void TwoPlayerMode::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Vẽ maze (dùng toạ độ player1 để vẽ ánh sáng)
    maze.render(renderer, player1.getX(), player1.getY());

    // Vẽ player1 & player2
    player1.render(renderer);
    player2.render(renderer);

    SDL_RenderPresent(renderer);
}

void TwoPlayerMode::checkWinCondition() {
    SDL_Rect p2Rect = player2.getRect();
    SDL_Rect goalRect = { maze.getGoalX(), maze.getGoalY(), 40, 40 };

    // Player2 phải có key để thắng
    if (SDL_HasIntersection(&p2Rect, &goalRect)) {
        if (player2.hasKey()) {
            running = false;
        }
    }
}

