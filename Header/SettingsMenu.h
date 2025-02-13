#ifndef SETTINGS_MENU_H
#define SETTINGS_MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <map>
#include <string>

class SettingsMenu {
public:
    SettingsMenu(SDL_Renderer* renderer);
    ~SettingsMenu();

    int run(); // Chạy menu
    void saveSettings(); // Lưu cài đặt vào file
    void loadSettings(); // Đọc cài đặt từ file

private:
    SDL_Renderer* renderer;
    TTF_Font* font;

    int volume; // Giá trị âm lượng (0-100)
    std::map<std::string, SDL_Keycode> keybinds; // Map lưu keybind

    SDL_Rect volumeSlider; // Thanh trượt âm lượng
    SDL_Rect volumeHandle; // Nút kéo của slider
    SDL_Rect keybindRects[2]; // Ô nhập cho 2 keybinds
    bool selectingKey[2]; // Xác định ô nào đang được chọn

    SDL_Rect saveButton; // Nút Save

    bool draggingVolume;

    // Hiệu ứng nhấp nháy
    int selectedItem;
    int blinkTimer;
    bool blinkState;

    void render(); // Vẽ menu
    void handleEvent(SDL_Event& e); // Xử lý sự kiện
    SDL_Texture* renderText(const std::string& text); // Tạo texture từ văn bản
};

#endif
