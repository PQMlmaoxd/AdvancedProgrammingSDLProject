#pragma once

#include <SDL.h>
#include "Maze.h"
#include "Player.h"

class TwoPlayerMode {
public:
    TwoPlayerMode(SDL_Renderer* renderer);
    ~TwoPlayerMode();

    // Hàm chạy game loop của chế độ 2 người chơi
    int run();

private:
    SDL_Renderer* renderer;

    // Mê cung dùng chung
    Maze maze;

    // Hai người chơi
    Player player1;
    Player player2;

    // Cờ để chạy vòng lặp
    bool running = false;

    // Các hàm xử lý vòng lặp
    void handleEvents();
    void update();
    void render();

    // Kiểm tra thắng cuộc
    void checkWinCondition();
    int returnValue;
    Uint32 startTime2P;
};
