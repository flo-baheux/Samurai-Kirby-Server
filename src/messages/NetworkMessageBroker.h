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

  private:
  MessageHub &messageHub;

  // For now lets try with id -> one callback, not multiple ones
  // NB: Could restrict visibility of this to prmessage friend class from directly accessing it.
  std::unordered_map<int, GameplayMessageCallback> gameplayMessageCallbackByPlayerId;

  void dispatchMessages();
};

#endif // NETWORKMESSAGEHUB_H
