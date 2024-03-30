#pragma once

#include <string>
#include <nlohmann/json.hpp>

#include "Room.h"
#include "GameplayMessages.h"

class Serializer {
public:
  static const std::string serializeGameplayMessage(const GameplayMessage &);
  static const std::shared_ptr<PlayerActionMessage> deserializePlayerActionMessage(const std::string &);

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
  static const std::string serialize(const PlayerReplayReadyStateChangedMessage &);
  static const std::string serialize(const AllReplayReadyGameStartingMessage &);

  static const nlohmann::json serializeGameResult(GameResultDTO gameResult);

  static const JoinGamePlayerActionMessage deserializeJoinGamePlayerActionMessage(nlohmann::json &);
  static const SetReadyStatePlayerActionMessage deserializeSetReadyStatePlayerActionMessage(nlohmann::json &);
  static const NotifyCanPlayPlayerActionMessage deserializeNotifyCanPlayPlayerActionMessage(nlohmann::json &);
  static const InputPlayerActionMessage deserializeInputPlayerActionMessage(nlohmann::json &);
  static const SetReplayReadyStatePlayerActionMessage deserializeSetReplayReadyStatePlayerActionMessage(nlohmann::json &);
  static const LeaveGamePlayerActionMessage deserializeLeaveGamePlayerActionMessage(nlohmann::json &);
};

class SerializeException : public nlohmann::json::exception {
public:
  explicit SerializeException(const nlohmann::json::exception &jsonException)
    : nlohmann::json::exception(jsonException) {};
};

class SerializeNotHandledException : public std::runtime_error {
public:
  explicit SerializeNotHandledException()
    : std::runtime_error("Serialize error - Unhandled message type") {};
};

class DeserializeException : public nlohmann::json::exception {
public:
  explicit DeserializeException(const nlohmann::json::exception &jsonException)
    : nlohmann::json::exception(jsonException) {};
};

class DeserializeNotHandledException : public std::runtime_error {
public:
  explicit DeserializeNotHandledException()
    : std::runtime_error("Deserialize error - Unhandled message type") {};
};
