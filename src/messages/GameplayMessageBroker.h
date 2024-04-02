#ifndef GAMEPLAYMESSAGEBROKER_H
#define GAMEPLAYMESSAGEBROKER_H

#include <vector>
#include <unordered_map>
#include <functional>
#include "MessageHub.h"

using PlayerConnectivityCallback = std::function<void(PlayerConnectivityMessage)>;
using PlayerActionCallback = std::function<void(int, std::shared_ptr<PlayerActionMessage>)>;

class GameplayMessageBroker {
public:
  using PlayerActionCallbackWithContext = std::pair<void *, PlayerActionCallback>;

  GameplayMessageBroker(MessageHub &messageHub) : messageHub{messageHub} {};

  GameplayMessageBroker(GameplayMessageBroker const &) = delete;
  void operator=(GameplayMessageBroker const &) = delete;

  void publishGameplayMessage(int playerId, std::shared_ptr<GameplayMessage> message);

  void subscribeToPlayerConnectivityMessages(PlayerConnectivityCallback callback);
  void subscribeToPlayerActionMessages(int playerId, PlayerActionCallback callback, void *context);
  void unsubscribeToPlayerActionMessages(int playerId, void *context);

  // TODO: Dispatch should be in a different class to avoid messageBroker holders to be able to dispatch
  void dispatchMessages();

private:
  MessageHub &messageHub;
  std::function<void(PlayerConnectivityMessage)> connectivityCallback;
  std::unordered_map<int, std::vector<PlayerActionCallbackWithContext>> playerActionCallbacksWithContextByPlayerId;

  friend class Gameplay;
};

#endif // GAMEPLAYMESSAGEBROKER_H
