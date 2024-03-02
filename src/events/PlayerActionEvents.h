#ifndef PLAYERACTIONEVENTS_H
#define PLAYERACTIONEVENTS_H

#include <string>
#include <iostream>
#include <vector>

#include "GameDifficulty.h"

enum PlayerActionEventType
{
  JOIN_GAME,
  SET_READY_STATE,
  NOTIFY_CAN_PLAY,
  PRESSED_BUTTON,
  SET_REPLAY_READY_STATE,
  LEAVE_GAME,
};

enum PlayerAssignment
{
  PLAYER_1,
  PLAYER_2
};

enum ButtonPressed
{
  A = 'A',
  B = 'B',
  X = 'X',
  Y = 'Y'
};

class PlayerActionEvent
{
protected:
  explicit PlayerActionEvent(PlayerActionEventType eventType)
      : eventType{eventType} {};

public:
  virtual ~PlayerActionEvent(){};
  const PlayerActionEventType eventType;
};

class JoinGamePlayerActionEvent : public PlayerActionEvent
{
public:
  JoinGamePlayerActionEvent(std::string playerName, GameDifficulty gameDifficulty)
      : PlayerActionEvent(PlayerActionEventType::JOIN_GAME),
        playerName{playerName},
        gameDifficulty{gameDifficulty} {};

  const std::string playerName;
  const GameDifficulty gameDifficulty;
};

class SetReadyStatePlayerActionEvent : public PlayerActionEvent
{
public:
  SetReadyStatePlayerActionEvent(bool isReady)
      : PlayerActionEvent(PlayerActionEventType::SET_READY_STATE),
        isReady{isReady} {};

  const bool isReady;
};

class NotifyCanPlayPlayerActionEvent : public PlayerActionEvent
{
public:
  NotifyCanPlayPlayerActionEvent()
      : PlayerActionEvent(PlayerActionEventType::NOTIFY_CAN_PLAY){};
};

class PressedButtonPlayerActionEvent : public PlayerActionEvent
{
public:
  PressedButtonPlayerActionEvent(ButtonPressed buttonPressed, int timeToPress)
      : PlayerActionEvent(PlayerActionEventType::PRESSED_BUTTON),
        buttonPressed{buttonPressed},
        timeToPress{timeToPress} {};

  const ButtonPressed buttonPressed;
  const int timeToPress;
};

class SetReplayReadyStatePlayerActionEvent : public PlayerActionEvent
{
public:
  SetReplayReadyStatePlayerActionEvent(bool isReadyToReplay)
      : PlayerActionEvent(PlayerActionEventType::SET_REPLAY_READY_STATE),
        isReadyToReplay{isReadyToReplay} {};

  const bool isReadyToReplay;
};

class LeaveGamePlayerActionEvent : public PlayerActionEvent
{
public:
  LeaveGamePlayerActionEvent()
      : PlayerActionEvent(PlayerActionEventType::LEAVE_GAME){};
};

// template <typename T_PlayerActionEvent, typename = std::enable_if_t<std::is_base_of_v<PlayerActionEvent, T_PlayerActionEvent>>>
// void processEvent(T_PlayerActionEvent e)
// {
//   std::cout << "processing player event" << std::endl;
// }

#endif // PLAYERACTIONEVENTS_H