#include <algorithm>
#include "SDL2/SDL.h"
#include "player.h"

Player::Player(int x_offset, int y_offset) {
  this->x_offset = x_offset;
  this->y_offset = y_offset;
}

Player::~Player() {

}

void Player::addCardToHand(Card *card) {
  this->hand.push_back(card);
}

void Player::clearHand() {
  this->hand.clear();
}

void Player::clearTable(std::function<void()> callback) {
//  for(Card *c : this->table) {
//    c->setTarget(560, 600, [&] (const Card& card) {
////      this->table.clear();
//      callback();
//    });
//  }
  this->table.clear();
}

void Player::removeCardFromHand(Card *card) {
//  this->hand.erase(std::remove(this->hand.begin(), this->hand.end(), card), this->hand.end());
}

void Player::addCardToTable(Card *card) {
  this->table.push_back(card);
  int x = static_cast<int>(this->table.size()) - 1;
  int tX = (x % 3) * 100 + this->x_offset;
  int tY = (x / 3) * 100 + this->y_offset;
  card->setTarget(tX + tY/10, tY);
}

int Player::getScore() {
  int total = 0;
  for(Card *card : this->table) {
    total += card->getValue();
  }
  return total;
}

std::pair<Constants::ActionType, Card*> Player::makeMove(Player *opponent) {

  // The other player is standing.
  if(opponent->getAction() == Constants::ActionType::STAND) {
    // If my score is greater than the opponents quit!
    if(this->getScore() > opponent->getScore()) {
      return std::make_pair(Constants::ActionType::STAND, (Card*)nullptr);
    }
  }

  for (Card* c : this->getHand()) {
    if(c->getValue() > 0) {
      int value = this->getScore() + c->getValue();
      if(value >= 17 && value <= 20 && !c->getUsed()) {
        return std::make_pair(Constants::ActionType::PLAY, c);
      }
    }
  }


  if(this->getScore() <= 17) {
    return std::make_pair(Constants::ActionType::END_TURN, (Card*) nullptr);
  }


  return std::make_pair(Constants::ActionType::STAND, (Card*) nullptr);
}

void Player::update(int delta) {
  for(Card *card : this->table) {
    card->update(delta);
  }
}

void Player::render(SDL_Renderer *r, TTF_Font *font) {
  for(Card *card : this->table) {
    card->render(r, font);
  }
  for(Card *card : this->hand) {
    card->render(r, font);
  }
}

void Player::setAction(Constants::ActionType a) {
  this->action = a;
}

std::vector<Card*> Player::getHand() {
  return this->hand;
}

Constants::ActionType Player::getAction() {
  return this->action;
}

