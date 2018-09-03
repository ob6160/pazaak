#ifndef BUTTON_H
#define BUTTON_H

#include <functional>
#include <iostream>
#include "SDL2/SDL.h"
#include "constants.h"

using namespace Constants;

class Button {
  public:
    Button(int x, int y, int width, int height, std::function<void(const Button&)> fn, SDL_Color colour = WHITE, std::string text = "");
    Button(SDL_Rect dimensions, std::function<void(const Button&)> fn, SDL_Color colour = WHITE, std::string text = "");
    ~Button() {}
    bool checkIntersection(int x, int y);
    void render(SDL_Renderer *r, TTF_Font *font);
    void load(SDL_Renderer *r);
    void update(int delta);
    bool handleEvent(SDL_Event event);
  private:
    SDL_Rect dimensions;
    SDL_Texture *buttonTexture, *buttonTextureHover;
    SDL_Color colour;
    std::string text, imgPath = "button.png", imgPathHover = "button_hover.png";
    bool hover = false;
    bool click = false;
    std::function<void(const Button&)> clickCallback;
};

#endif
