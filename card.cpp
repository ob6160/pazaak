#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "card.h"
#include <utility>
#include "helpers.h"
#include "constants.h"

using namespace Constants;

/**
 * Constructor
 *
 * @param value
 * @param dimensions
 */
Card::Card(int value, SDL_Rect dimensions, std::string imgPath) {
  this->value = value;
  this->dimensions = dimensions;
  this->originalPos = dimensions;
  this->imgPath = std::move(imgPath);

  this->tX = dimensions.x;
  this->tY = dimensions.y;
}

/**
 * Loads the card template image from the filesystem into an SDL texture.
 *
 * @param r
 */
void Card::load(SDL_Renderer *r) {
  this->cardTexture = LoadImage(r, this->imgPath);
}

Card::~Card() {

}

int Card::getValue() {
  return this->value;
}

void Card::update(int delta) {
  if(this->dealing) {
    double dX, dY;
    dX = this->originalPos.x - this->target.x;
    dY = this->originalPos.y - this->target.y;

    tX -= dX*.05;
    tY -= dY*.05;

    double dist = sqrt((this->target.x - tX)*(this->target.x - tX) + (this->target.y - tY)*(this->target.y - tY));
    if(dist == 0) {
      this->dimensions.x = this->target.x;
      this->dimensions.y = this->target.y;
      this->tX = this->dimensions.x;
      this->tY = this->dimensions.y;
      this->originalPos.x = this->dimensions.x;
      this->originalPos.y = this->dimensions.y;
      if(this->targetCallback) {
        this->targetCallback(*this);
      }
      this->dealing = false;
    } else {
      this->dimensions.x = static_cast<int>(round(tX));
      this->dimensions.y = static_cast<int>(round(tY));
    }
  }
}

void Card::handleEvent(SDL_Event event) {
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
        this->selected = !this->selected;

      } else if(this->click) {
        this->click = false;
      }
  }
}

bool Card::getSelected() {
  return this->selected;
}

bool Card::getUsed() {
  return this->used;
}

void Card::setUsed(bool used) {
  this->used = used;
}

void Card::unselect() {
  this->selected = false;
}


bool Card::checkIntersection(int x, int y) {
  SDL_Rect clickRect;
  clickRect.w = clickRect.h = 1;
  clickRect.x = x;
  clickRect.y = y;

  SDL_Rect cardCheck{this->dimensions.x + 50, this->dimensions.y + 25, 100,150};

  return SDL_HasIntersection(&clickRect, &cardCheck);
}

/**
 * Renders the card image and overlays the text value.
 *
 * @param r
 * @param font
 */
void Card::render(SDL_Renderer *r, TTF_Font *font) {
  if(this->selected) {
    SDL_SetRenderDrawColor(r, 0,255,0,1);
    SDL_Rect cardCheck{this->dimensions.x + 50, this->dimensions.y + 25, 100,150};
    SDL_RenderFillRect(r, &cardCheck);
    SDL_SetRenderDrawColor(r, 0,0,0,1);
  }

  SDL_RenderCopy(r, this->cardTexture, NULL, &dimensions);

  if(this->value != NULL && !this->getUsed()) {
    RenderText(
        r
        ,std::to_string(this->value)
        ,font
        ,BLACK
        ,SDL_Rect{dimensions.x + 95, dimensions.y + 100, 0, 0}
        ,1.5
    );
  }
}

/**
 * Set the dimensions of the card.
 *
 * @param newDimen
 */
void Card::setDimensions(SDL_Rect newDimen) {
  this->dimensions = newDimen;
}

/**
 * Set the x,y position of the card.
 *
 * @param x
 * @param y
 */
void Card::setPosition(int x, int y) {
  this->dimensions.x = x;
  this->dimensions.y = y;
}

/**
 * ..
 *
 * @param x
 * @param y
 */
void Card::setTarget(int x, int y, std::function<void(const Card&)> callback, bool deal) {
  this->target.x = x;
  this->target.y = y;
  this->targetCallback = callback;
  this->dealing = deal;
}
