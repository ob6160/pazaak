#include <iostream>
#include <SDL_image.h>
#include <functional>
#include "button.h"
#include "SDL2/SDL.h"
#include "helpers.h"


Button::Button(SDL_Rect dimensions, std::function<void(const Button&)> fn, SDL_Color colour, std::string text) {
  this->dimensions = dimensions;
  this->colour = colour;
  this->text = std::move(text);
  this->clickCallback = fn;
}

Button::Button(int x, int y, int width, int height, std::function<void(const Button&)> fn, SDL_Color colour, std::string text) {
  this->dimensions.x = x;
  this->dimensions.y = y;
  this->dimensions.w = width;
  this->dimensions.h = height;
  this->colour = colour;
  this->text = std::move(text);
  this->clickCallback = fn;
}

/**
 * Loads the button template image from the filesystem into an SDL texture.
 *
 * @TODO: Both Card and Button implement the same method - unify into one class.
 *
 * @param r
 */
void Button::load(SDL_Renderer *r) {
  this->buttonTexture = LoadImage(r, this->imgPath);
  this->buttonTextureHover = LoadImage(r, this->imgPathHover);
}

void Button::render(SDL_Renderer *r, TTF_Font *font) {
  if(hover) {
    SDL_RenderCopy(r, this->buttonTextureHover, NULL, &dimensions);
  } else {
    SDL_RenderCopy(r, this->buttonTexture, NULL, &dimensions);
  }
  RenderText(
      r
      , this->text
      , font
      , Constants::BLACK
      , SDL_Rect{
            static_cast<int>(this->dimensions.x + this->dimensions.w * 0.5)
          , static_cast<int>(this->dimensions.y + this->dimensions.h * 0.4)
          , static_cast<int>(this->dimensions.w * 0.75)
          , static_cast<int>(this->dimensions.h * 0.75)}
      , 1.0
      , true
  );
}

void Button::update(int delta) {

}

bool Button::handleEvent(SDL_Event event) {
  

  switch(event.type) {
    case SDL_MOUSEMOTION:
      this->hover = this->checkIntersection(event.motion.x, event.motion.y);
      break;
    case SDL_MOUSEBUTTONDOWN:
      if(this->checkIntersection(event.button.x, event.button.y)) {
        this->click = true;
      }
      break;
    case SDL_MOUSEBUTTONUP:
      if(this->click && this->checkIntersection(event.button.x, event.button.y)) {
        this->click = false;
        this->clickCallback(*this);
        return true;
      } else if(this->click) {
        this->click = false;
      }
  }

  return false;
}


bool Button::checkIntersection(int x, int y) {
  SDL_Rect clickRect;
  clickRect.w = clickRect.h = 1;
  clickRect.x = x;
  clickRect.y = y;
  return SDL_HasIntersection(&clickRect, &this->dimensions);
}