#include "RoomManager.h"

RoomManager::RoomManager(GameplayMessageBroker &messageBroker) : messageBroker{messageBroker} {};

void RoomManager::addPlayerToRoom(int playerId, std::string nickname, GameDifficulty difficulty) {
  auto room = std::find_if(rooms.begin(), rooms.end(),
    [&](auto const &room) { return room->isFull() == false && room->config.difficulty == difficulty; }
  );

  if (room != rooms.end())
    (*room)->addPlayer(playerId, nickname);
  else {
    std::unique_ptr<Room> room = std::make_unique<Room>(messageBroker, Room::RoomConfig{difficulty});
    room->addPlayer(playerId, nickname);
    rooms.push_back(std::move(room));
  }
};

void RoomManager::removePlayerFromRoom(int playerId) {
  auto roomPlayerIsIn = std::find_if(rooms.begin(), rooms.end(),
    [&](auto const &room) { return room->hasPlayerWithId(playerId); }
  );

  if (roomPlayerIsIn != rooms.end()) {
    (*roomPlayerIsIn)->removePlayer(playerId);
    if ((*roomPlayerIsIn)->isEmpty())
      roomPlayerIsIn = rooms.erase(roomPlayerIsIn);
    return;
  }
};

void RoomManager::tickAllRooms() {
  for (auto &room : rooms)
    room->tick();
};

void RoomManager::handlePlayerMessage(int playerId, std::shared_ptr<PlayerActionMessage> message) {
  switch (message->messageType) {
    case JOIN_GAME:
      {
        auto msg = std::dynamic_pointer_cast<JoinGamePlayerActionMessage>(message);
        addPlayerToRoom(playerId, msg->nickname, msg->gameDifficulty);
      }
      break;
    case LEAVE_GAME:
      removePlayerFromRoom(playerId);
      break;
    default:
      break;
  }
}