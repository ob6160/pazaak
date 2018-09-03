#ifndef CARD_H
#define CARD_H

#include <iostream>
#include <SDL_ttf.h>
#include <functional>
#include "SDL2/SDL_image.h"

class Card {
  public:
    explicit Card(int value = 0, SDL_Rect dimensions = {0,0,200,200}, std::string imgPath = "card_tpl.png");
    ~Card();
    void render(SDL_Renderer *r, TTF_Font *font);
    void setDimensions(SDL_Rect newDimen);
    void setPosition(int x, int y);
    void setTarget(int x, int y, std::function<void(const Card&)> callback = nullptr, bool deal = true);
    int getValue();
    void update(int delta);
    void load(SDL_Renderer *r);
    void handleEvent(SDL_Event event);
    bool getSelected();
    void unselect();
    bool getUsed();
    void setUsed(bool used);
private:
  int value;
  std::string imgPath;
  bool dealing = true;
  SDL_Rect dimensions, target, originalPos;
  double tX, tY;
  SDL_Texture *cardTexture;
  bool checkIntersection(int x, int y);
  bool hover;
  bool click;
  bool selected = false;
  bool used = false;
  std::function<void(const Card &)> targetCallback = nullptr;
};

#endif
