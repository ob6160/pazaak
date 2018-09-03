#include "pazaak.h"
#include <map>

Pazaak::Pazaak(TTF_Font *font, int upperLimit, int lowerLimit, int individualCount) {
  this->font = font;
  this->upperLimit = upperLimit;
  this->lowerLimit = lowerLimit;
  this->individualCount = individualCount;

  this->endTurn = new Button(SCREEN_WIDTH - 200,625,150,50, [&] (const Button& b) {
    if(this->playerTurn == PlayerType::PLAYER_ONE) {
      this->gameController(ActionType::END_TURN);
    }

  }, KOTOR_DARK_GREEN, "End Turn");

  this->stand = new Button(SCREEN_WIDTH - 200,685,150,50, [&] (const Button& b) {
    if(this->playerTurn == PlayerType::PLAYER_ONE) {
      this->gameController(ActionType::STAND);
    }
  }, KOTOR_DARK_GREEN, "Stand");

  this->play = new Button(SCREEN_WIDTH - 200,745,150,50, [&] (const Button& b) {
    if(this->playerTurn == PlayerType::PLAYER_ONE) {
      this->waitForHandChoice = true;
    }
  }, KOTOR_DARK_GREEN, "Play");

  this->dismissStatus = new Button(SCREEN_WIDTH / 2 - 32
      ,SCREEN_HEIGHT / 2 + 50,152,50, [&] (const Button& b) {
    this->showStatusMessage = false;

    if(this->matchOver) {
      this->matchOver = false;
      this->p1score = 0;
      this->p2score = 0;

      this->computerPlayer->clearHand();
      this->humanPlayer->clearHand();

      this->initialiseHand(this->renderer);
    }

    this->nextRound(this->renderer);
  }, KOTOR_DARK_GREEN, "Close");

  this->humanPlayer = new Player(0,50);
  this->computerPlayer = new Player(500,50);

  this->players = std::map<PlayerType, Player*>{
    std::pair<PlayerType, Player*>(PlayerType::PLAYER_ONE, this->humanPlayer),
    std::pair<PlayerType, Player*>(PlayerType::PLAYER_TWO, this->computerPlayer)
  };
}

Pazaak::~Pazaak() {

}

Player* Pazaak::getPlayer(PlayerType p) {
  return this->players[p];
}

Player* Pazaak::swapPlayerTurn() {
  if(this->playerTurn == PlayerType::PLAYER_ONE) {
    this->playerTurn = PlayerType::PLAYER_TWO;
    this->computerTimer.reset();
  } else {
    this->playerTurn = PlayerType::PLAYER_ONE;
  }
  return this->getPlayer(this->playerTurn);
}

void Pazaak::gameController(ActionType action, Card* playCard) {
  Player* currentPlayer = this->getPlayer(this->playerTurn);
  if(action == ActionType::STAND) {
    currentPlayer->setAction(ActionType::STAND);
  } else if(action == ActionType::END_TURN) {
    currentPlayer->setAction(ActionType::END_TURN);
  } else if(action == ActionType::PLAY) {
    currentPlayer->setAction(ActionType::PLAY);
    currentPlayer->addCardToTable(new Card(*playCard));
    playCard->setUsed(true);
    return;
  }

  if(this->handleRoundEnd()) return;

  Player* nextPlayer = this->swapPlayerTurn();
  // If the next player is standing swap back to the non-standing one.
  if(nextPlayer->getAction() == ActionType::STAND) {
    nextPlayer = this->swapPlayerTurn();
  }

  this->deal(nextPlayer);

  if(this->handleRoundEnd()) return;
}

bool Pazaak::handleRoundEnd() {
  if(this->checkRoundEnd()) {
    this->state = GameState::PAUSE;

    if(this->humanPlayer->getAction() == ActionType::STAND && this->computerPlayer->getAction() == ActionType::STAND) {
      if(this->humanPlayer->getScore() > this->computerPlayer->getScore()) {
        this->statusMessage = "You won the round!";
        this->showStatusMessage = true;
        this->p1score++;
      } else if(this->computerPlayer->getScore() > this->humanPlayer->getScore()){
        this->statusMessage = "Computer Won!";
        this->showStatusMessage = true;
        this->p2score++;
      } else {
        this->statusMessage = "It was a draw!";
        this->showStatusMessage = true;
      }
    }

    if(this->computerPlayer->getScore() == this->humanPlayer->getScore()) {
      this->statusMessage = "It was a draw!";
      this->showStatusMessage = true;
    }

    if(this->humanPlayer->getScore() > 20 && this->computerPlayer->getScore() <= 20) {
      this->statusMessage = "Computer Won!";
      this->showStatusMessage = true;
      this->p2score++;
      // compPlayer won.
    } else if(this->computerPlayer->getScore() > 20 && this->humanPlayer->getScore() <= 20) {
      // humanPlayer won.
      this->statusMessage = "You won the round!";
      this->showStatusMessage = true;
      this->p1score++;
    }

    if(this->p1score == 3) {
      this->statusMessage = "You win match!";
      this->showStatusMessage = true;
      this->matchOver = true;
    } else if(this->p2score == 3) {
      this->statusMessage = "Computer wins match!";
      this->showStatusMessage = true;
      this->matchOver = true;
    }

    return true;
  }
  return false;
}

void Pazaak::nextRound(SDL_Renderer *r) {
  for(auto const& [key, val] : this->players) {
    val->clearTable([&] () {

    });
  }

  this->computerPlayer->setAction(ActionType::PLAY);
  this->humanPlayer->setAction(ActionType::PLAY);

  this->deck.clear();

  this->initialiseDeck(r);

  this->playerTurn = (rand()%2) ? PlayerType::PLAYER_ONE : PlayerType::PLAYER_TWO;
  this->deal(this->players[this->playerTurn]);

  this->state = GameState::PLAY;

}

bool Pazaak::checkRoundEnd() {
  bool roundEnd = false;
  // If both players stand then consider the round over.
  if(this->computerPlayer->getAction() == ActionType::STAND && this->humanPlayer->getAction() == ActionType::STAND) {
    roundEnd = true;
  }
  // If either player has a score above 20 consider the round over.
  if(this->computerPlayer->getScore() > 20 || this->humanPlayer->getScore() > 20) {
    roundEnd = true;
  }

  return roundEnd;
};

void Pazaak::setup(SDL_Renderer *r) {
  this->renderer = r;

  // Setup and deal to first player
  this->initialiseDeck(r);

  this->initialiseHand(r);

  // Setup two buttons.
  this->endTurn->load(r);
  this->stand->load(r);
  this->play->load(r);
  this->dismissStatus->load(r);

  this->score = LoadImage(r, "score.png");
  this->messageBorder = LoadImage(r, "message_border.png");

  this->nextRound(r);
}

void Pazaak::deal(Player* p) {
  if(this->deck.size() - 1 == 0) {
    return;
  }

  // Pick a random element from the deck.
  Card *pick;
  int randPick = static_cast<int>(rand() % this->deck.size());
  // Make a copy of it.
  pick = this->deck[randPick];

  // Swap the dealt card with the last and pop back to remove it.
  std::swap(this->deck[randPick], this->deck.back());
  this->deck.pop_back();

  p->addCardToTable(new Card(*pick));

  std::cout << "Adding card : " << pick->getValue() << std::endl;
  std::cout << "New deck size : " << this->deck.size() << std::endl;
}

void Pazaak::initialiseDeck(SDL_Renderer *r) {
  this->deckCard = new Card(0, SDL_Rect{500,500,200,200}, "card_back_tpl.png");
  deckCard->load(r);

  // Add two of each card within the upper and lower limit to the deck.
  for(int i = this->lowerLimit; i <= this->upperLimit; i++) {
    for(int j = 0; j < this->individualCount; j++) {
      auto *newCard = new Card(i, SDL_Rect{560,600,200,200});
      newCard->load(r);
      this->deck.push_back(newCard);
    }
  }
}

void Pazaak::initialiseHand(SDL_Renderer *r) {
  // TODO: Allow players to choose the cards that make up their hand.
  for(int i = 0; i < 4; i++) {
    int rand_1 = (rand()%2) ? randRange(-5, -1) : randRange(1,5);
    int rand_2 = (rand()%2) ? randRange(-5, -1) : randRange(1,5);
    Card* newHCard = new Card(rand_1, SDL_Rect{i * 100, 600,200,200});
    Card* newCCard = new Card(rand_2, SDL_Rect{-1000, -1000,200,200});
    newHCard->load(r);
    newCCard->load(r);

    this->humanPlayer->addCardToHand(newHCard);
    this->computerPlayer->addCardToHand(newCCard);
  }

}

void Pazaak::render(SDL_Renderer *r) {
  std::string p1text = (this->playerTurn == PlayerType::PLAYER_ONE) ? "> You" : "You";
  std::string p2text = (this->playerTurn == PlayerType::PLAYER_TWO) ? "> Computer" : "Computer";
  std::string p1score = "Your Score: " + std::to_string(this->p1score);
  std::string p2score = "Computuer Score: " + std::to_string(this->p2score);
  RenderText(r, p1text, this->font, Constants::WHITE, SDL_Rect{50,0,0,0}, 2.0, false);
  RenderText(r, p1score, this->font, Constants::WHITE, SDL_Rect{32,800,0,0}, 2.0, false);
  RenderText(r, p2text, this->font, Constants::WHITE, SDL_Rect{550,0,0,0}, 2.0, false);
  RenderText(r, p2score, this->font, Constants::WHITE, SDL_Rect{32,850,0,0}, 2.0, false);
  RenderText(r, "Your Hand", this->font, Constants::WHITE, SDL_Rect{50,550,0,0}, 2.0, false);
  RenderText(r, "Deck", this->font, Constants::WHITE, SDL_Rect{600,550,0,0}, 2.0, false);

  SDL_Rect controlsPos{SCREEN_WIDTH - 225, 560, 200, 300};
  SDL_RenderCopy(r, this->score, NULL, &controlsPos);

  this->deckCard->setPosition(560, 600);
  this->deckCard->render(r, this->font);

  this->stand->render(r, this->font);
  this->endTurn->render(r, this->font);
  this->play->render(r, this->font);

  for(auto const& [key, val] : this->players) {
    val->render(r, this->font);
  }

  SDL_Rect scorePosP1{364,16,64,64};
  SDL_RenderCopy(r, this->score, NULL, &scorePosP1);
  RenderText(
      r
      , std::to_string(this->players[PlayerType::PLAYER_ONE]->getScore())
      , this->font
      , Constants::BLACK
      , SDL_Rect{scorePosP1.x + 32, scorePosP1.y + 28, 0, 0}
      , 1.5
      , true
  );
  SDL_Rect scorePosP2{864,16,64,64};
  SDL_RenderCopy(r, this->score, NULL, &scorePosP2);
  RenderText(
      r
      , std::to_string(this->players[PlayerType::PLAYER_TWO]->getScore())
      , this->font
      , Constants::BLACK
      , SDL_Rect{scorePosP2.x + 32, scorePosP2.y + 28, 0, 0}
      , 1.5
      , true
  );

  if(this->showStatusMessage) {
    SDL_Rect statusPos{SCREEN_WIDTH / 2 - 216, SCREEN_HEIGHT / 2 - 137, 512, 256};
    SDL_RenderCopy(r, this->messageBorder, NULL, &statusPos);
    this->dismissStatus->render(r, this->font);
    RenderText(
      r
      ,this->statusMessage
      ,font
      ,BLACK
      ,SDL_Rect{statusPos.x + 256, statusPos.y + 100, 0, 0},
      2.0
    );
  }
}

void Pazaak::update(int delta) {
  for(auto const& [key, val] : this->players) {
    val->update(delta);
  }

  if(this->state == GameState::PLAY) {
    if(this->playerTurn == PlayerType::PLAYER_ONE && this->waitForHandChoice) {
      for(Card* card : this->getPlayer(PlayerType::PLAYER_ONE)->getHand()) {
        if(card->getSelected() && !card->getUsed()) {
          card->unselect();
          this->waitForHandChoice = false;
          this->gameController(ActionType::PLAY, card);
        }
      }
    }

    if(this->playerTurn == PlayerType::PLAYER_TWO) {
      if(this->computerTimer.getDiff() > randRange(1000,1500)) {
        auto nextMove = this->getPlayer(this->playerTurn)->makeMove(this->getPlayer(PlayerType::PLAYER_ONE));
        this->gameController(nextMove.first, nextMove.second);
        this->computerTimer.reset();
      }
    }
  }
}

void Pazaak::handleEvent(SDL_Event event) {
  bool btnClick = false;

  // Check to see if the click was over a button.
  btnClick |= this->endTurn->handleEvent(event);
  btnClick |= this->stand->handleEvent(event);
  btnClick |= this->play->handleEvent(event);

  if(this->showStatusMessage) {
    btnClick |= this->dismissStatus->handleEvent(event);
  }

  bool clicked = false;
  switch(event.type) {
    case SDL_MOUSEBUTTONDOWN:
      this->click = true;
      break;
    case SDL_MOUSEBUTTONUP:
      if(this->click) {
        this->click = false;
        clicked = true;
      } else if(this->click) {
        this->click = false;
      }
  }

  for(Card* card : this->humanPlayer->getHand()) {
    if(clicked && !btnClick) card->unselect();
    card->handleEvent(event);
  }
}

