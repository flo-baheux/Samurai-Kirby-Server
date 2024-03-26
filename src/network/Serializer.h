#pragma once

#include <string>
#include <nlohmann/json.hpp>

#include "Room.h"
#include "GameplayMessages.h"

class Serializer {
public:
  static const std::string serialize(const GameplayMessage &);

private:
  static const std::string serialize(const RoomJoinedMessage &);
  static const std::string serialize(const PlayerJoinedRoomMessage &);
  static const std::string serialize(const PlayerLeftRoomMessage &);
  static const std::string serialize(const PlayerReadyStateChangedMessage &);
  static const std::string serialize(const AllReadyGameStartingMessage &);
  static const std::string serialize(const GameExpectingInputMessage &);
  static const std::string serialize(const GameOverEarlyInputMessage &);
  static const std::string serialize(const GameOverWrongInputMessage &);
  static const std::string serialize(const GameOverWinMessage &);
  static const std::string serialize(const GameOverDrawMessage &);
  // static const std::string serialize(const PlayerReplayReadyStateChangedMessage &);
  // static const std::string serialize(const AllReplayReadyGameStartingMessage &);
  // static const std::string serialize(const GameOverDrawMessage &);

  const static nlohmann::json serializeGameResult(GameResultDTO gameResult);
};
