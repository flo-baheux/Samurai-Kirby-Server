#include "GameplayMessageBroker.h"

void GameplayMessageBroker::subscribeToPlayerConnectivityMessages(PlayerConnectivityMessageCallback callback)
{
  connectivityMessageCallback = callback;
};

void GameplayMessageBroker::subscribeToPlayerActionMessages(int playerId, PlayerActionMessageCallback callback)
{
  playerActionMessageCallbacksByPlayerId[playerId].push_back(callback);
}

// Note: This method does not scale well.
// This is fine for now (performances can be evaluated)
// However in a context with more concurrent users we could use a threadpool + playerId sharding
void GameplayMessageBroker::dispatchMessages()
{
  while (!messageHub.playerConnectivityMessages.empty())
  {
    connectivityMessageCallback(messageHub.playerConnectivityMessages.front());
    messageHub.playerConnectivityMessages.pop();
  }

  std::vector<int> playerIdsWithListeners;
  for (const auto &pair : playerActionMessageCallbacksByPlayerId)
  {
    const int playerIdWithListeners = pair.first;
    const auto callbackList = pair.second;
    auto &messageQueueForPlayerId = messageHub.playerActionMessagesByPlayerId[playerIdWithListeners];
    while (!messageQueueForPlayerId.empty())
    {
      for (const auto &callback : callbackList)
      {
        callback(playerIdWithListeners, messageQueueForPlayerId.front());
      }
      messageQueueForPlayerId.pop();
    }
  }
}

void GameplayMessageBroker::publishGameplayMessage(int playerId, std::shared_ptr<GameplayMessage> message)
{
  messageHub.gameplayMessagesByPlayerId[playerId].push(message);
}
