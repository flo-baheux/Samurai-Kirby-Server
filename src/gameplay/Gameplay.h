#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <memory>
#include "GameplayMessageBroker.h"
#include "Room.h"
#include "RoomManager.h"

class Gameplay {
public:
  Gameplay(std::unique_ptr<GameplayMessageBroker> messageBroker);
  ~Gameplay() {};

  Gameplay(const Gameplay &) = delete;
  Gameplay &operator=(const Gameplay &) = delete;

  void start();

private:
  std::unique_ptr<GameplayMessageBroker> messageBroker;
  RoomManager roomManager;

  void handlePlayerConnectivityMessage(PlayerConnectivityMessage message);
};

#endif // GAMEPLAY_H