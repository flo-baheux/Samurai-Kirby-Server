#pragma once
#include <functional>
#include "catch2/catch.hpp"

#include "MessageHub.h"
#include "GameplayMessageBroker.h"

class GameplayMessageTestUtils {
public:
  GameplayMessageTestUtils(MessageHub &messageHub, GameplayMessageBroker &messageBroker)
    : messageHub{messageHub}, messageBroker{messageBroker} {};

  MessageHub &messageHub;
  GameplayMessageBroker &messageBroker;

  int countMessagesPendingForPlayer(int playerId);
  void removeAllMessagesPendingForPlayer(int playerId);
  void publishDispatchActionMessageFromPlayer(int playerId, std::shared_ptr<PlayerActionMessage>);
  std::shared_ptr<GameplayMessage> consumeOneGameplayMessageForPlayer(int playerId);

  template <typename T_GameplayMessage>
  inline T_GameplayMessage unwrapGameplayMessage(std::shared_ptr<GameplayMessage> wrappedMessage) {
    return dynamic_cast<T_GameplayMessage &>(*wrappedMessage);
  }
};

