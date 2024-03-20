#include "Player.h"

void Player::resetToReplay()
{
  isReady = true;
  canPlay = false;
  isReadyToReplay = false;
  inputPressed = std::nullopt;
  timeToPress = std::nullopt;
};

const PlayerLobbyDTO Player::buildLobbyDTO() const
{
  return {
      nickname,
      assignment,
      isReady,
  };
};

const PlayerGameResultDTO Player::buildGameoverDTO() const
{
  return {
      assignment,
      inputPressed,
      timeToPress,
  };
};
