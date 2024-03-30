#include "Gameplay.h"

Gameplay::Gameplay(std::unique_ptr<GameplayMessageBroker> messageBroker)
  : messageBroker{std::move(messageBroker)}, roomManager{*Gameplay::messageBroker} {
  Gameplay::messageBroker->subscribeToPlayerConnectivityMessages(
    [this](PlayerConnectivityMessage message) { handlePlayerConnectivityMessage(message); });
};

void Gameplay::start() {
  while (true) {
    // tick on each room
    messageBroker->dispatchMessages();
    roomManager.tickAllRooms();
  }
}

void Gameplay::handlePlayerConnectivityMessage(PlayerConnectivityMessage message) {
  switch (message.messageType) {
    case CONNECT:
      std::cout << "new player with id [" << message.playerId << "]" << std::endl;
      messageBroker->subscribeToPlayerActionMessages(message.playerId,
        [this](int playerId, std::shared_ptr<PlayerActionMessage> e) {
          roomManager.handlePlayerMessage(playerId, e);
        });
      break;
    case DISCONNECT:
      roomManager.removePlayerFromRoom(message.playerId);
      // TODO Unsub to player action messages
      break;
    default:
      break;
  }
}