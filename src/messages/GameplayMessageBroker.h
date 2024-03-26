#ifndef GAMEPLAYMESSAGEBROKER_H
#define GAMEPLAYMESSAGEBROKER_H

#include <vector>
#include <unordered_map>
#include <functional>
#include "MessageHub.h"

using PlayerActionMessageCallback = std::function<void(int, std::shared_ptr<PlayerActionMessage>)>;
using PlayerConnectivityMessageCallback = std::function<void(PlayerConnectivityMessage)>;

class GameplayMessageBroker {
public:
  GameplayMessageBroker(MessageHub &messageHub) : messageHub{messageHub} {};

  GameplayMessageBroker(GameplayMessageBroker const &) = delete;
  void operator=(GameplayMessageBroker const &) = delete;

  void publishGameplayMessage(int playerId, std::shared_ptr<GameplayMessage> message);

  void subscribeToPlayerConnectivityMessages(PlayerConnectivityMessageCallback callback);
  void subscribeToPlayerActionMessages(int playerId, PlayerActionMessageCallback callback);

  // private:
    // Dispatch should be in a different class to avoid messageBroker holders to be able to dispatch
  void dispatchMessages();

private:
  MessageHub &messageHub;
  std::function<void(PlayerConnectivityMessage)> connectivityMessageCallback;
  std::unordered_map<int, std::vector<PlayerActionMessageCallback>> playerActionMessageCallbacksByPlayerId;

  friend class Gameplay;
};

#endif // GAMEPLAYMESSAGEBROKER_H
