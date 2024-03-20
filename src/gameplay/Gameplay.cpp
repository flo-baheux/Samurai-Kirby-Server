#include "Gameplay.h"

Gameplay::Gameplay(std::unique_ptr<GameplayMessageBroker> messageBroker)
    : messageBroker{std::move(messageBroker)}
{
  Gameplay::messageBroker->subscribeToPlayerConnectivityMessages(
      [this](PlayerConnectivityMessage message)
      { handlePlayerConnectivityMessage(message); });
};

void Gameplay::start()
{
  while (true)
  {
    // tick on each room
    messageBroker->dispatchMessages();
    for (const std::unique_ptr<Room> &room : rooms)
      room->tick();
  }
}

void Gameplay::handlePlayerConnectivityMessage(PlayerConnectivityMessage message)
{
  switch (message.messageType)
  {
  case CONNECT:
    std::cout << "new player with id [" << message.playerId << "]" << std::endl;
    playerIds.push_back(message.playerId);
    messageBroker->subscribeToPlayerActionMessages(message.playerId,
                                                   [this](int playerId, std::shared_ptr<PlayerActionMessage> e)
                                                   { handlePlayerMessages(playerId, e); });
    break;
  case DISCONNECT:
    playerIds.erase(std::remove(playerIds.begin(), playerIds.end(), message.playerId), playerIds.end());
    break;
  default:
    break;
  }
}

void Gameplay::handlePlayerMessages(int playerId, std::shared_ptr<PlayerActionMessage> message)
{
  if (std::find(playerIds.begin(), playerIds.end(), playerId) == playerIds.end())
  {
    std::cout << "ERROR: unknown player id " << playerId << std::endl;
    return;
  }

  if (message->messageType == JOIN_GAME)
    handlePlayerJoinMessage(playerId, std::dynamic_pointer_cast<JoinGamePlayerActionMessage>(message));
}

void Gameplay::handlePlayerJoinMessage(int playerId, std::shared_ptr<JoinGamePlayerActionMessage> message)
{
  GameDifficulty difficulty = message->gameDifficulty;
  std::cout << "Handling player join message for player"
            << playerId << " [" << message->nickname
            << "]" << std::endl;

  auto it = std::find_if(rooms.begin(), rooms.end(), [&](auto const &room)
                         { return room->isFull() == false && room->difficulty == difficulty; });

  if (it != rooms.end())
  {
    (*it)->addPlayer(playerId, message->nickname);
    return;
  }
  else
  {
    std::unique_ptr<Room> room = std::make_unique<Room>(*messageBroker, message->gameDifficulty);
    room->addPlayer(playerId, message->nickname);
    rooms.push_back(std::move(room));
  }
}
