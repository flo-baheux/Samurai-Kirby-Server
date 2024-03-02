#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <memory>
#include "GameplayEventBroker.h"

class Gameplay
{
public:
  Gameplay(std::unique_ptr<GameplayEventBroker> eventBroker)
      : eventBroker{std::move(eventBroker)} {};
  ~Gameplay(){};

  Gameplay(const Gameplay &) = delete;
  Gameplay &operator=(const Gameplay &) = delete;

  void start();

private:
  std::unique_ptr<GameplayEventBroker> eventBroker;

  std::vector<Player *> players;
};

#endif // GAMEPLAY_H