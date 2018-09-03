#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include <cstdio>
#include <iostream>

#include "pazaak.h"
#include "button.h"
#include "constants.h"
#include "helpers.h"

using namespace std;
using namespace Constants;

int main(int argc, char *argv[])
{
  srand(time(NULL));

  // Game variable setup.
  SDL_Window* gameWindow = nullptr;
  SDL_Renderer* gameRenderer = nullptr;

  int tick_begin, tick_time = 0;
  int frameCount = 0;
  int fps_tick = SDL_GetTicks();
  float fps = 0.0f;

  /**
   * Initialisation
   */

  TTF_Init();

  TTF_Font* gameFont = TTF_OpenFont("alterebro-pixel-font.ttf", 42);
  if(!gameFont) {
    printf("Font load error: %s\n", TTF_GetError());
  }

  int initStatus = SDL_Init(SDL_INIT_VIDEO);
  if(initStatus != 0) {
    printf("SDL Error: %s\n", SDL_GetError());
    return -1;
  }

  gameWindow = SDL_CreateWindow(
      "Pazaak",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      SCREEN_WIDTH,
      SCREEN_HEIGHT,
      SDL_WINDOW_SHOWN
  );
  if(gameWindow == NULL)
  {
    printf("SDL Error: %s\n", SDL_GetError());
    return -1;
  }

  gameRenderer = SDL_CreateRenderer(gameWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

  if(!IMG_Init(IMG_INIT_PNG)) {
    printf("SDL_IMG Error: %s\n", IMG_GetError());
    return -1;
  }

  /**
   * End Initialisation
   */

  // Game instance setup.
  Pazaak *pazaakInstance = new Pazaak(gameFont);
  pazaakInstance->setup(gameRenderer);

  // Main game loop.
  while(true) {
    tick_begin = SDL_GetTicks();

    // Event handling
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      pazaakInstance->handleEvent(event);
      switch(event.type) {
        case SDL_QUIT:
          return 0;
      }
    }

    // Clear the screen to black.
    SDL_SetRenderDrawColor(gameRenderer, 0, 0, 0, 0);
    SDL_RenderClear(gameRenderer);
    {
      pazaakInstance->render(gameRenderer);
    }
    SDL_RenderPresent(gameRenderer);

    frameCount++;

    // Calculate elapsed time since last iteration.
    tick_time = SDL_GetTicks() - tick_begin;

    pazaakInstance->update(tick_time);

    // If a total of a second has passed since the last FPS sample make another.
    if(SDL_GetTicks() - fps_tick > 1000.0) {
      // Calculate FPS and log to terminal.
      fps = (float) frameCount / (SDL_GetTicks() - fps_tick);
      fps_tick = SDL_GetTicks();
      frameCount = 0;
      std::cout << "FPS: " << fps*1000 << std::endl;
    }

    // If the tick time took less than the max frame time artificially delay for consistent FPS.
    if(FPS_DELAY > tick_time) {
      SDL_Delay(FPS_DELAY - tick_time);
    }
  }

  SDL_DestroyWindow(gameWindow);
  SDL_DestroyRenderer(gameRenderer);
}
