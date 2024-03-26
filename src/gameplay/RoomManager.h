#pragma once

#include <memory>
#include <map>
#include "GameplayMessageBroker.h"
#include "Room.h"

class RoomManager {

public:
  RoomManager(GameplayMessageBroker &messageBroker);
  ~RoomManager() {};
  RoomManager(const RoomManager &) = delete;
  RoomManager &operator=(const RoomManager &) = delete;

  void tickAllRooms();

  void addPlayerToRoom(int playerId, std::string nickname, GameDifficulty difficulty);
  void removePlayerFromRoom(int playerId);
  void handlePlayerMessage(int playerId, std::shared_ptr<PlayerActionMessage> message);


private:
  GameplayMessageBroker &messageBroker;
  std::vector<std::unique_ptr<Room>> rooms;
};