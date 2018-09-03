#ifndef PAZAAK_H
#define PAZAAK_H

#include <vector>
#include <iostream>
#include <cstdlib>
#include <map>

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_events.h"

#include "player.h"
#include "card.h"
#include "constants.h"
#include "button.h"
#include "helpers.h"

class Pazaak {
  public:
    explicit Pazaak(TTF_Font *font, int upperLimit = 10, int lowerLimit = 1, int individualCount = 2);
    ~Pazaak();
    void initialiseDeck(SDL_Renderer *r);
    void deal(Player *p);
    void setup(SDL_Renderer *r);
    void render(SDL_Renderer *r);
    void update(int delta);
    void handleEvent(SDL_Event event);
    void gameController(ActionType action, Card* playCard = nullptr);
    Player *getPlayer(PlayerType p);
    int p1score = 0, p2score = 0;

private:
    SDL_Renderer *renderer;
    bool matchOver = false;
    std::vector<Card*> deck, hand;
    Card *deckCard;
    int upperLimit, lowerLimit, individualCount;
    Constants::PlayerType playerTurn = PlayerType::PLAYER_ONE;
    TTF_Font *font;
    Button *endTurn, *stand, *play, *dismissStatus;
    SDL_Texture *score;
    Player * swapPlayerTurn();
    Player *humanPlayer;
    Player *computerPlayer;
    std::map<PlayerType, Player *> players;
    Timer computerTimer;
    GameState state = GameState::PLAY;
    bool waitForHandChoice = false;
    bool showStatusMessage = false;
    std::string statusMessage = "test message";

    bool checkWinConditions();

    bool checkRoundEnd();

    void initialiseHand(SDL_Renderer *r);

    bool handleRoundEnd();
  SDL_Texture *messageBorder;

  void nextRound();

  void nextRound(SDL_Renderer *r);

  bool click;
};

#endif
