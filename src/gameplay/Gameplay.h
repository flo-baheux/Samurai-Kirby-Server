#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <memory>
#include "GameplayMessageBroker.h"
#include "Room.h"

class Gameplay
{
public:
  Gameplay(std::unique_ptr<GameplayMessageBroker> messageBroker);
  ~Gameplay(){};

  Gameplay(const Gameplay &) = delete;
  Gameplay &operator=(const Gameplay &) = delete;

  void start();

  void handlePlayerMessages(int playerId, std::shared_ptr<PlayerActionMessage> message);

private:
  std::unique_ptr<GameplayMessageBroker> messageBroker;

  void handlePlayerConnectivityMessage(PlayerConnectivityMessage message);
  void handlePlayerJoinMessage(int playerId, std::shared_ptr<JoinGamePlayerActionMessage> message);

  std::vector<int> playerIds;
  std::vector<std::unique_ptr<Room>> rooms;
};

#endif // GAMEPLAY_H