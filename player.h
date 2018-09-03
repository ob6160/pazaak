#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include "card.h"
#include "constants.h"

class Player {
  public:
    Player(int x_offset, int y_offset = 0);
    ~Player();
    void addCardToHand(Card *card);
    void addCardToTable(Card *card);
    void render(SDL_Renderer *r, TTF_Font *font);
    void update(int delta);
    int getScore();
    void setAction(Constants::ActionType a);
    Constants::ActionType getAction();
    std::vector<Card *> getHand();
    std::pair<Constants::ActionType, Card*> makeMove(Player *opponent);
    void removeCardFromHand(Card *card);
    void clearHand();
    void clearTable(std::function<void()> callback);
private:
  std::vector<Card*> table;
  std::vector<Card*> hand;

  int upperLimit, lowerLimit, individualCount, x_offset, y_offset;
  Constants::ActionType action;
protected:

};

#endif
