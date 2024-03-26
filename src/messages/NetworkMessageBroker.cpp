#include "NetworkMessageBroker.h"

void NetworkMessageBroker::publishPlayerConnectivityMessage(PlayerConnectivityMessage message) {
  messageHub.playerConnectivityMessages.push(message);
};

void NetworkMessageBroker::publishPlayerActionMessage(int playerId, std::shared_ptr<PlayerActionMessage> message) {
  messageHub.playerActionMessagesByPlayerId[playerId].push(message);
}

void NetworkMessageBroker::subscribeToGameplayMessagesForPlayer(int playerId, GameplayMessageCallback callback) {
  gameplayMessageCallbackByPlayerId[playerId] = callback;
}

void NetworkMessageBroker::dispatchMessages() {
  std::vector<int> playerIdsWithListeners;
  for (const auto &pair : gameplayMessageCallbackByPlayerId) {
    const int playerId = pair.first;
    const auto callback = pair.second;

    auto &messageQueueForPlayerId = messageHub.gameplayMessagesByPlayerId[playerId];

    while (!messageQueueForPlayerId.empty()) {
      callback(messageQueueForPlayerId.front());
      messageQueueForPlayerId.pop();
    }
  }
}