#ifndef PAZAAK_CONSTANTS_H
#define PAZAAK_CONSTANTS_H

#include <SDL_ttf.h>

namespace Constants {
  #define FPS 60
  #define FPS_DELAY 1000 / FPS

  #define SCREEN_WIDTH 1000
  #define SCREEN_HEIGHT 1000

  const SDL_Color WHITE = {255, 255, 255};
  const SDL_Color BLACK = {0, 0, 0};
  const SDL_Color KOTOR_DARK_GREEN = {36,74,26};

  enum class PlayerType {
    PLAYER_ONE, PLAYER_TWO
  };

  enum class ActionType {
    END_TURN,
    STAND,
    PLAY
  };

  enum class GameState {
    PLAY,
    PAUSE,
  };
}

#endif //PAZAAK_CONSTANTS_H
