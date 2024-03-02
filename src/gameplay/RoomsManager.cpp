#include <algorithm>
#include <iostream>

#include "RoomsManager.h"

GameplayRoom *RoomsManager::joinAvailableRoom(ConnectedClient *player, GameDifficulty difficulty)
{
  GameplayRoom *availableRoom = findGameplayRoomWaitingForPlayer(difficulty);

  if (!availableRoom)
  {
    GameplayRoom *newRoom = new GameplayRoom{difficulty};

    rooms.push_back(newRoom);
    availableRoom = newRoom;
  }
  availableRoom->addPlayer(player);
  return availableRoom;
};

GameplayRoom *RoomsManager::findGameplayRoomWaitingForPlayer(GameDifficulty difficulty)
{
  auto findRoomPredicate = [&](auto const room)
  {
    return room->isFull() == false && room->getRoomDifficulty() == difficulty;
  };

  auto iterator = std::find_if(rooms.begin(), rooms.end(), findRoomPredicate);
  if (iterator != rooms.end())
  {
    return *iterator;
  }

  return nullptr;
};

void RoomsManager::deleteEmptyRooms()
{
  auto iterator = rooms.begin();
  while (iterator != rooms.end())
  {
    if ((*iterator)->isEmpty())
    {

      delete *iterator;
      iterator = rooms.erase(iterator);
      std::cout << "deleted a room!" << std::endl;
    }
    else
    {
      iterator++;
    }
  }
}
