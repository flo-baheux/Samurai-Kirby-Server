#ifndef PLAYERACTIONMESSAGES_H
#define PLAYERACTIONMESSAGES_H

#include <string>
#include <iostream>
#include <vector>

#include "GameDifficulty.h"
#include "GameplayInput.h"

enum PlayerActionMessageType
{
  JOIN_GAME,
  SET_READY_STATE,
  NOTIFY_CAN_PLAY,
  PLAYER_INPUT,
  SET_REPLAY_READY_STATE,
  LEAVE_GAME,
};

class PlayerActionMessage
{
protected:
  explicit PlayerActionMessage(PlayerActionMessageType messageType)
      : messageType{messageType} {};

public:
  virtual ~PlayerActionMessage(){};
  const PlayerActionMessageType messageType;
};

class JoinGamePlayerActionMessage : public PlayerActionMessage
{
public:
  JoinGamePlayerActionMessage(std::string nickname, GameDifficulty gameDifficulty)
      : PlayerActionMessage(JOIN_GAME),
        nickname{nickname},
        gameDifficulty{gameDifficulty} {};

  const std::string nickname;
  const GameDifficulty gameDifficulty;
};

class SetReadyStatePlayerActionMessage : public PlayerActionMessage
{
public:
  SetReadyStatePlayerActionMessage(bool isReady)
      : PlayerActionMessage(SET_READY_STATE),
        isReady{isReady} {};

  const bool isReady;
};

class NotifyCanPlayPlayerActionMessage : public PlayerActionMessage
{
public:
  NotifyCanPlayPlayerActionMessage()
      : PlayerActionMessage(NOTIFY_CAN_PLAY){};
};

class InputPlayerActionMessage : public PlayerActionMessage
{
public:
  InputPlayerActionMessage(GameplayInput inputPressed, int timeToPress)
      : PlayerActionMessage(PLAYER_INPUT),
        inputPressed{inputPressed},
        timeToPress{timeToPress} {};

  const GameplayInput inputPressed;
  const int timeToPress;
};

class SetReplayReadyStatePlayerActionMessage : public PlayerActionMessage
{
public:
  SetReplayReadyStatePlayerActionMessage(bool isReadyToReplay)
      : PlayerActionMessage(SET_REPLAY_READY_STATE),
        isReadyToReplay{isReadyToReplay} {};

  const bool isReadyToReplay;
};

class LeaveGamePlayerActionMessage : public PlayerActionMessage
{
public:
  LeaveGamePlayerActionMessage()
      : PlayerActionMessage(LEAVE_GAME){};
};

#endif // PLAYERACTIONMESSAGES_H