#ifndef KEY_H
#define KEY_H

#include <SDL.h>
#include <string>

class Key {
public:
    Key();
    ~Key();

    // Khởi tạo vị trí và tải texture
    bool loadTexture(SDL_Renderer* renderer, const std::string& filePath);
    void setPosition(int x, int y);
    SDL_Rect getRect() const;
    void render(SDL_Renderer* renderer);

private:
    SDL_Texture* keyTexture;
    SDL_Rect rect;  // Vị trí và kích thước của key (ví dụ: 40x40)
};

#endif
