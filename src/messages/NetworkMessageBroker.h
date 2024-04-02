#ifndef NETWORKMESSAGEHUB_H
#define NETWORKMESSAGEHUB_H

#include <vector>
#include <functional>
#include <memory>
#include "MessageHub.h"

using GameplayMessageCallback = std::function<void(std::shared_ptr<GameplayMessage>)>;

class NetworkMessageBroker {
  friend class Server;

public:
  NetworkMessageBroker(MessageHub &messageHub) : messageHub{messageHub} {};

  NetworkMessageBroker(NetworkMessageBroker const &) = delete;
  void operator=(NetworkMessageBroker const &) = delete;

  void publishPlayerConnectivityMessage(PlayerConnectivityMessage message);
  void publishPlayerActionMessage(int playerId, std::shared_ptr<PlayerActionMessage> message);

  void subscribeToGameplayMessagesForPlayer(int playerId, GameplayMessageCallback callback);
  void unsubscribeToGameplayMessagesForPlayer(int playerId);

private:
  MessageHub &messageHub;

  std::unordered_map<int, GameplayMessageCallback> gameplayMessageCallbackByPlayerId;

  void dispatchMessages();
};

#endif // NETWORKMESSAGEHUB_H
