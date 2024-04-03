#include "GameplayMessageBroker.h"

void GameplayMessageBroker::subscribeToPlayerConnectivityMessages(PlayerConnectivityCallback callback) {
  connectivityCallback = callback;
};

void GameplayMessageBroker::subscribeToPlayerActionMessages(int playerId, PlayerActionCallback callback, void *context) {

  playerActionCallbacksWithContextByPlayerId[playerId].push_back(std::make_pair(context, callback));
}

void GameplayMessageBroker::unsubscribeToPlayerActionMessages(int playerId, void *context) {
  std::vector<PlayerActionCallbackWithContext> callbacksWithContext = playerActionCallbacksWithContextByPlayerId[playerId];

  for (auto it = callbacksWithContext.begin(); it != callbacksWithContext.end();) {
    PlayerActionCallbackWithContext &callbackWithContext = *it;
    if (callbackWithContext.first == context)
      it = callbacksWithContext.erase(it);
    else
      it++;
  }
}

// Note: This method does not scale well.
// This is fine for now.
// However in a context with more concurrent users we could use a threadpool, playerId sharding, etc.
void GameplayMessageBroker::dispatchMessages() {
  while (!messageHub.playerConnectivityMessages.empty()) {
    connectivityCallback(messageHub.playerConnectivityMessages.front());
    messageHub.playerConnectivityMessages.pop();
  }

  for (const auto &pair : playerActionCallbacksWithContextByPlayerId) {
    const int playerIdWithListeners = pair.first;
    const auto callbacksWithContext = pair.second;
    auto &messageQueueForPlayerId = messageHub.playerActionMessagesByPlayerId[playerIdWithListeners];
    while (!messageQueueForPlayerId.empty()) {
      for (const auto &callbackWithContext : callbacksWithContext) {
        try {
          callbackWithContext.second(playerIdWithListeners, messageQueueForPlayerId.front());
        } catch (std::exception &error) {
          // TODO: Logger
          std::cerr << "Error while calling callback:" << error.what() << std::endl;
        }
      }
      messageQueueForPlayerId.pop();
    }
  }
}

void GameplayMessageBroker::publishGameplayMessage(int playerId, std::shared_ptr<GameplayMessage> message) {
  messageHub.gameplayMessagesByPlayerId[playerId].push(message);
}
