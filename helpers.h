#ifndef PAZAAK_HELPERS_H
#define PAZAAK_HELPERS_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include <iostream>

class Timer {
  public:
    Timer() {
      this->beginTime = SDL_GetTicks();
    }
    inline int getDiff() {
      return SDL_GetTicks() - this->beginTime;
    }
    inline void reset() {
      this->beginTime = SDL_GetTicks();
    }
  private:
    int beginTime;

};

inline int randRange(int a, int b) {
  return rand() % (b - a) + a;
}

inline SDL_Texture* LoadImage(SDL_Renderer *r, std::string path) {
  SDL_Surface *baseImage = IMG_Load(path.c_str());
  if (baseImage == NULL) {
    printf("SDL_IMG Error: %s\n", IMG_GetError());
  }
  SDL_Texture *newTex = SDL_CreateTextureFromSurface(r, baseImage);
  SDL_FreeSurface(baseImage);
  return newTex;
}

inline void RenderText(SDL_Renderer* r, std::string text, TTF_Font *textFont, SDL_Color colour, SDL_Rect rect, double scale = 0, bool offset = true) {
  // Setup surface and texture.
  SDL_Surface* renderedTextSurface = TTF_RenderText_Solid(textFont, text.c_str(), colour);
  SDL_Texture* renderedTextTexture = SDL_CreateTextureFromSurface(r, renderedTextSurface);

  // Optionally scale and offset the text.
  if(scale > 0) {
    rect.w = (int) ceil(renderedTextSurface->w * scale);
    rect.h = (int) ceil(renderedTextSurface->h * scale);
  }

  // Offset text.
  if(offset) {
    rect.x -= rect.w/2;
    rect.y -= rect.h/2;
  }

  SDL_RenderCopy(r, renderedTextTexture, NULL, &rect);

  SDL_FreeSurface(renderedTextSurface);
  SDL_DestroyTexture(renderedTextTexture);
}

inline SDL_Surface* RenderTextToSurface(std::string text, TTF_Font *textFont, SDL_Color colour) {
  return TTF_RenderText_Solid(textFont, text.c_str(), colour);
}

#endif //PAZAAK_HELPERS_H
