#ifndef GAMEPLAYEVENTS_H
#define GAMEPLAYEVENTS_H

#include <string>
#include <iostream>
#include <vector>

#include "GameDifficulty.h"

enum GameplayEventType
{

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

class GameplayEvent
{
protected:
  explicit GameplayEvent(GameplayEventType eventType)
      : eventType{eventType} {};

public:
  virtual ~GameplayEvent(){};
  const GameplayEventType eventType;
};

// class JoinGameGameplayEvent : public GameplayEvent
// {
// public:
//   JoinGameGameplayEvent(std::string playerName, GameDifficulty gameDifficulty)
//       : GameplayEvent(GameplayEventType::JOIN_GAME),
//         playerName{playerName},
//         gameDifficulty{gameDifficulty} {};

//   const std::string playerName;
//   const GameDifficulty gameDifficulty;
// };

// class SetReadyStateGameplayEvent : public GameplayEvent
// {
// public:
//   SetReadyStateGameplayEvent(bool isReady)
//       : GameplayEvent(GameplayEventType::SET_READY_STATE),
//         isReady{isReady} {};

//   const bool isReady;
// };

// class NotifyCanPlayGameplayEvent : public GameplayEvent
// {
// public:
//   NotifyCanPlayGameplayEvent(PlayerAssignment playerAssignment)
//       : GameplayEvent(GameplayEventType::NOTIFY_CAN_PLAY){};
// };

// class PressedButtonGameplayEvent : public GameplayEvent
// {
// public:
//   PressedButtonGameplayEvent(PlayerAssignment playerAssignment, ButtonPressed buttonPressed)
//       : GameplayEvent(GameplayEventType::PRESSED_BUTTON), buttonPressed{buttonPressed} {};

//   const ButtonPressed buttonPressed;
// };

// class SetReplayReadyStateGameplayEvent : public GameplayEvent
// {
// public:
//   SetReplayReadyStateGameplayEvent(PlayerAssignment playerAssignment, bool isReadyToReplay)
//       : GameplayEvent(GameplayEventType::SET_REPLAY_READY_STATE), isReadyToReplay{isReadyToReplay} {};

//   const bool isReadyToReplay;
// };

// class LeaveGameGameplayEvent : public GameplayEvent
// {
// public:
//   LeaveGameGameplayEvent(PlayerAssignment playerAssignment)
//       : GameplayEvent(GameplayEventType::LEAVE_GAME){};
// };

// template <typename T_GameplayEvent, typename = std::enable_if_t<std::is_base_of_v<GameplayEvent, T_GameplayEvent>>>
// void processEvent(T_GameplayEvent e)
// {
//   std::cout << "processing player event" << std::endl;
// }

#endif // GAMEPLAYEVENTS_H