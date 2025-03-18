#include "Utils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <fstream>
#include <iostream>

std::string promptForSaveName(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_StartTextInput();
    std::string saveName = "";
    bool done = false;
    SDL_Event e;

    while (!done) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                done = true;
                break;
            }
            if (e.type == SDL_TEXTINPUT) {
                // Giới hạn 15 ký tự
                if (saveName.size() < 15) {
                    saveName += e.text.text;
                }
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_BACKSPACE && !saveName.empty()) {
                    saveName.pop_back();
                }
                if (e.key.keysym.sym == SDLK_RETURN) {
                    done = true;
                }
            }
        }

        // Xoá màn hình
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Color white = {255, 255, 255, 255};

        // 1) Vẽ prompt "Nhap ten file save:"
        {
            std::string prompt = "Nhap ten file save:";
            SDL_Surface* promptSurface = TTF_RenderUTF8_Solid(font, prompt.c_str(), white);
            SDL_Texture* promptTexture = SDL_CreateTextureFromSurface(renderer, promptSurface);
            SDL_FreeSurface(promptSurface);

            // Lấy kích thước thật của texture
            int promptW = 0, promptH = 0;
            SDL_QueryTexture(promptTexture, NULL, NULL, &promptW, &promptH);

            // Căn giữa theo chiều ngang (giả sử cửa sổ rộng 800)
            SDL_Rect promptRect;
            promptRect.x = (800 - promptW) / 2;
            promptRect.y = 200;   // Toạ độ Y tuỳ ý
            promptRect.w = promptW;
            promptRect.h = promptH;

            SDL_RenderCopy(renderer, promptTexture, NULL, &promptRect);
            SDL_DestroyTexture(promptTexture);

            // 2) Vẽ dòng người dùng đã nhập (saveName)
            SDL_Surface* inputSurface = TTF_RenderUTF8_Solid(font, saveName.c_str(), white);
            SDL_Texture* inputTexture = SDL_CreateTextureFromSurface(renderer, inputSurface);
            SDL_FreeSurface(inputSurface);

            int inputW = 0, inputH = 0;
            SDL_QueryTexture(inputTexture, NULL, NULL, &inputW, &inputH);

            // Đặt ngay dưới prompt, cách khoảng 40px
            SDL_Rect inputRect;
            inputRect.x = (800 - inputW) / 2;
            inputRect.y = promptRect.y + promptRect.h + 40;
            inputRect.w = inputW;
            inputRect.h = inputH;

            SDL_RenderCopy(renderer, inputTexture, NULL, &inputRect);
            SDL_DestroyTexture(inputTexture);
        }

        // Hiển thị
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    SDL_StopTextInput();

    // Flush các sự kiện còn lại (tránh Enter thừa)
    SDL_FlushEvent(SDL_TEXTINPUT);
    SDL_FlushEvent(SDL_KEYDOWN);

    // Kiểm tra tên file hợp lệ
    if (saveName.empty()) {
        std::cout << "Ten file khong duoc de trong!\n";
        return "";
    }
    if (saveName.find('/') != std::string::npos || saveName.find('\\') != std::string::npos) {
        std::cout << "Ten file chua ky tu khong hop le!\n";
        return "";
    }

    // Kiểm tra xem file đã tồn tại hay chưa
    std::string fullPath = "Save/" + saveName + ".txt";
    std::ifstream file(fullPath);
    if (file.good()) {
        std::cout << "File da ton tai!\n";
        return "";
    }
    return saveName;
}
