#ifndef GAMEPLAYMESSAGES_H
#define GAMEPLAYMESSAGES_H

#include <string>
#include <iostream>
#include <vector>

#include "GameDifficulty.h"
#include "PlayerAssignment.h"
#include "GameResultDTO.h"
#include "PlayerDTO.h"

enum GameplayMessageType
{
  ROOM_JOINED = 0,
  PLAYER_JOINED_GAME = 1,
  PLAYER_READY_STATE_CHANGED = 2,
  ALL_READY_GAME_STARTING = 3,
  GAME_EXPECTING_INPUT = 4,
  GAME_OVER_EARLY_INPUT = 5,
  GAME_OVER_WRONG_INPUT = 6,
  GAME_OVER_WIN = 7,
  GAME_OVER_DRAW = 8,
  PLAYER_REPLAY_READY_STATE_CHANGED = 9,
  ALL_REPLAY_READY_GAME_STARTING = 10,
};

class GameplayMessage
{
protected:
  explicit GameplayMessage(GameplayMessageType messageType)
      : messageType{messageType} {};

public:
  virtual ~GameplayMessage(){};
  const GameplayMessageType messageType;
};

class RoomJoinedMessage : public GameplayMessage
{
public:
  RoomJoinedMessage(PlayerAssignment playerAssignment, std::optional<PlayerLobbyDTO> otherPlayer = std::nullopt)
      : GameplayMessage(ROOM_JOINED),
        playerAssignment{playerAssignment},
        otherPlayer{otherPlayer} {};

  const PlayerAssignment playerAssignment;
  const std::optional<PlayerLobbyDTO> otherPlayer;
};

class PlayerJoinedRoomMessage : public GameplayMessage
{
public:
  PlayerJoinedRoomMessage(PlayerLobbyDTO player)
      : GameplayMessage(PLAYER_JOINED_GAME),
        player{player} {};

  const PlayerLobbyDTO player;
};

class PlayerReadyStateChangedMessage : public GameplayMessage
{
public:
  PlayerReadyStateChangedMessage(PlayerAssignment playerAssignment, bool isReady)
      : GameplayMessage(PLAYER_READY_STATE_CHANGED),
        playerAssignment{playerAssignment}, isReady{isReady} {};

  const PlayerAssignment playerAssignment;
  const bool isReady;
};

class AllReadyGameStartingMessage : public GameplayMessage
{
public:
  AllReadyGameStartingMessage()
      : GameplayMessage(ALL_READY_GAME_STARTING){};
};

class GameExpectingInputMessage : public GameplayMessage
{
public:
  GameExpectingInputMessage(GameplayInput expectedInput)
      : GameplayMessage(GAME_EXPECTING_INPUT),
        expectedInput{expectedInput} {};

  const GameplayInput expectedInput;
};

class GameOverEarlyInputMessage : public GameplayMessage
{
public:
  GameOverEarlyInputMessage(GameResultDTO gameResult)
      : GameplayMessage(GAME_OVER_EARLY_INPUT),
        gameResult{gameResult} {}

  const GameResultDTO gameResult;
};

class GameOverWrongInputMessage : public GameplayMessage
{
public:
  GameOverWrongInputMessage(GameResultDTO gameResult)
      : GameplayMessage(GAME_OVER_WRONG_INPUT),
        gameResult{gameResult} {}

  const GameResultDTO gameResult;
};

class GameOverWinMessage : public GameplayMessage
{
public:
  GameOverWinMessage(GameResultDTO gameResult)
      : GameplayMessage(GAME_OVER_WIN),
        gameResult{gameResult} {}

  const GameResultDTO gameResult;
};

class GameOverDrawMessage : public GameplayMessage
{
public:
  GameOverDrawMessage(GameResultDTO gameResult)
      : GameplayMessage(GAME_OVER_DRAW),
        gameResult{gameResult} {}

  const GameResultDTO gameResult;
};

class PlayerReplayReadyStateChangedMessage : public GameplayMessage
{
public:
  PlayerReplayReadyStateChangedMessage(PlayerAssignment playerAssignment, bool isReadyToReplay)
      : GameplayMessage(PLAYER_REPLAY_READY_STATE_CHANGED),
        playerAssignment{playerAssignment},
        isReadyToReplay{isReadyToReplay} {};

  const PlayerAssignment playerAssignment;
  const bool isReadyToReplay;
};

class AllReplayReadyGameStartingMessage : public GameplayMessage
{
public:
  AllReplayReadyGameStartingMessage()
      : GameplayMessage(ALL_REPLAY_READY_GAME_STARTING){};
};

#endif // GAMEPLAYMESSAGES_H