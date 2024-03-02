#ifndef ROOMSMANAGER_H
#define ROOMSMANAGER_H

#include "GameplayRoom.h"
#include "GameDifficulty.h"

class GameplayRoom;
class ConnectedClient;
class RoomsManager
{
public:
  GameplayRoom *joinAvailableRoom(ConnectedClient *player, GameDifficulty difficulty = GameDifficulty::HARD);

  void deleteEmptyRooms();

private:
  std::vector<GameplayRoom *> rooms{};

  GameplayRoom *findGameplayRoomWaitingForPlayer(GameDifficulty);
};

#endif