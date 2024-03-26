#include "gameplayMessageTestUtils.h"

std::shared_ptr<GameplayMessage> GameplayMessageTestUtils::consumeOneGameplayMessageForPlayer(int playerId) {
  auto message = messageHub.gameplayMessagesByPlayerId[playerId].front();
  messageHub.gameplayMessagesByPlayerId[playerId].pop();
  return message;
}

int GameplayMessageTestUtils::countMessagesPendingForPlayer(int playerId) {
  if (!messageHub.gameplayMessagesByPlayerId.contains(playerId))
    return 0;
  return (int) messageHub.gameplayMessagesByPlayerId[playerId].size();
}

void GameplayMessageTestUtils::removeAllMessagesPendingForPlayer(int playerId) {
  std::queue<std::shared_ptr<GameplayMessage>> empty;
  std::swap(messageHub.gameplayMessagesByPlayerId[playerId], empty);
}

void GameplayMessageTestUtils::publishDispatchActionMessageFromPlayer(int playerId, std::shared_ptr<PlayerActionMessage> message) {
  messageHub.playerActionMessagesByPlayerId[playerId].push(message);
  messageBroker.dispatchMessages();
}